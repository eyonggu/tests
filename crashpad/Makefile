src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

CXXFLAGS += -g -std=c++17 -I .. -I ../third_party/mini_chromium/mini_chromium -I ../out/Default/gen
LDFLAGS = -static ../out/Default/obj/client/libcommon.a ../out/Default/obj/client/libclient.a ../out/Default/obj/util/libutil.a ../out/Default/obj/third_party/mini_chromium/mini_chromium/base/libbase.a

myprog: $(obj)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	rm -f $(obj) myprog
