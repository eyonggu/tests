/*
 * Based on https://github.com/awilliam/tests
 */
#include <linux/types.h>
#include <linux/ioctl.h>

#include <linux/vfio.h>

#include <errno.h>
#include <libgen.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <linux/ioctl.h>

void usage(char *name)
{
	printf("usage: %s <ssss:bb:dd.f>\n", name);
}


static int vfio_get_iommu_group(char *dev_addr, int *group)
{
	char linkname[PATH_MAX];
	char filename[PATH_MAX];
	char *p;
	int ret;

	memset(linkname, 0, sizeof(linkname));
	memset(filename, 0, sizeof(filename));

	snprintf(linkname, sizeof(linkname), "/sys/bus/pci/devices/%s/iommu_group", dev_addr);

	ret = readlink(linkname, filename, sizeof(filename));
	if (ret < 0) {
		printf("Failed to readlink: %s (%d: %s)\n",
		       linkname, errno, strerror(errno));
		return ret;
	}

	p = strrchr(filename, '/') + 1;
	*group = strtol(p, NULL, 0);
	return 0;
}

int main(int argc, char **argv)
{
	int i, ret, container, group, device, groupid;
	char path[PATH_MAX];
	int seg, bus, dev, func;

	struct vfio_group_status group_status = {
		.argsz = sizeof(group_status)
	};

	struct vfio_device_info device_info = {
		.argsz = sizeof(device_info)
	};

	struct vfio_region_info region_info = {
		.argsz = sizeof(region_info)
	};

	if (argc < 2) {
		usage(argv[0]);
		return -1;
	}

	/*
	ret = sscanf(argv[1], "%d", &groupid);
	if (ret != 1) {
		usage(argv[0]);
		return -1;
	}
	*/

	ret = sscanf(argv[1], "%04x:%02x:%02x.%d", &seg, &bus, &dev, &func);
	if (ret != 4) {
		usage(argv[0]);
		return -1;
	}

	snprintf(path, sizeof(path), "%04x:%02x:%02x.%d", seg, bus, dev, func);
	ret = vfio_get_iommu_group(path, &groupid);
	if (ret) {
		printf("Failed to get iommu group for the device %s\n", path);
		return -1;
	}

	printf("Using PCI device %04x:%02x:%02x.%d in group %d\n",
               seg, bus, dev, func, groupid);

	container = open("/dev/vfio/vfio", O_RDWR);
	if (container < 0) {
		printf("Failed to open /dev/vfio/vfio, %d (%s)\n",
		       container, strerror(errno));
		return container;
	}

	snprintf(path, sizeof(path), "/dev/vfio/%d", groupid);
	group = open(path, O_RDWR);
	if (group < 0) {
		printf("Failed to open %s, %d (%s)\n",
		       path, group, strerror(errno));
		return group;
	}

	ret = ioctl(group, VFIO_GROUP_GET_STATUS, &group_status);
	if (ret) {
		printf("ioctl(VFIO_GROUP_GET_STATUS) failed\n");
		return ret;
	}

	if (!(group_status.flags & VFIO_GROUP_FLAGS_VIABLE)) {
		printf("Group not viable, are all devices attached to vfio?\n");
		return -1;
	}

	ret = ioctl(group, VFIO_GROUP_SET_CONTAINER, &container);
	if (ret) {
		printf("Failed to set group container\n");
		return ret;
	}

	ret = ioctl(container, VFIO_SET_IOMMU, VFIO_TYPE1_IOMMU);
	if (ret) {
		printf("Failed to set IOMMU\n");
		return ret;
	}

	snprintf(path, sizeof(path), "%04x:%02x:%02x.%d", seg, bus, dev, func);

	device = ioctl(group, VFIO_GROUP_GET_DEVICE_FD, path);
	if (device < 0) {
		printf("Failed to get device %s\n", path);
		return -1;
	}

	if (ioctl(device, VFIO_DEVICE_GET_INFO, &device_info)) {
		printf("Failed to get device info\n");
		return -1;
	}

	printf("Device supports %d regions, %d irqs\n",
	       device_info.num_regions, device_info.num_irqs);

	for (i = 0; i < device_info.num_regions; i++) {
		struct vfio_region_info *info;

		info = &region_info;
		printf("Region %d: ", i);
		region_info.index = i;
		if (ioctl(device, VFIO_DEVICE_GET_REGION_INFO, info)) {
			printf("Failed to get info\n");
			continue;
		}

		if (info->flags & VFIO_REGION_INFO_FLAG_CAPS) {
			info = calloc(info->argsz, 1);
			if (!info) {
				printf("Failed to alloc larger info\n");
				info = &region_info;
			} else {
				memcpy(info, &region_info, sizeof(region_info));
				if (ioctl(device,
					  VFIO_DEVICE_GET_REGION_INFO, info)) {
					printf("Failed to re-get info\n");
					continue;
				}
			}
		}

		printf("size 0x%lx, offset 0x%lx, flags 0x%x\n",
		       (unsigned long)info->size,
		       (unsigned long)info->offset, info->flags);
		if (0 && info->flags & VFIO_REGION_INFO_FLAG_MMAP) {
			void *map = mmap(NULL, (size_t)region_info.size,
					 PROT_READ, MAP_SHARED, device,
					 (off_t)region_info.offset);
			if (map == MAP_FAILED) {
				printf("mmap failed\n");
				continue;
			}

			printf("[");
			fwrite(map, 1, region_info.size > 16 ? 16 :
						region_info.size, stdout);
			printf("]\n");
			munmap(map, (size_t)region_info.size);
		}

		if (info->flags & VFIO_REGION_INFO_FLAG_CAPS) {
			struct vfio_info_cap_header *header;

			header = (void *)info + info->cap_offset;

next:
			printf("\tCapability @%d: ID %d, version %d, next %d\n",
			       info->cap_offset, header->id, header->version,
			       header->next);

			if (header->id == VFIO_REGION_INFO_CAP_SPARSE_MMAP) {
				struct vfio_region_info_cap_sparse_mmap *sparse;
				int i;

				sparse = (void *)header;

				printf("\t\tsparse mmap cap, nr_areas %d\n",
				       sparse->nr_areas);

				for (i = 0; i < sparse->nr_areas; i++)
					printf("\t\t\t%d: %lx-%lx\n", i,
					       sparse->areas[i].offset,
					       sparse->areas[i].offset +
					       sparse->areas[i].size);
			}

			if (header->next) {
				header = (void *)info + header->next;
				goto next;
			}
		}

		if (info != &region_info)
			free(info);
	}

	printf("Success\n");
	//printf("Press any key to exit\n");
	//fgetc(stdin);

	return 0;
}
