主要封装了两种类: CameraNode, ClientNode
CameraNode类通过守护线程不断grab解决了由BUFFER_SIZE引起的非实时帧
ClientNode类提供了一种包含帧头、帧尾、时间戳、消息类型和内容长度的通讯模式，并且将发送，接收运行在不同线程内
除此之外还实现了服务端和客户端的shutdown重连，以及为避免发送时间过长设计的timeout机制

ubuntu上安装opencv, yaml-cpp
修改CMakeLists,将server相关的行去注释
mkdir build
cd build
cmake ..
make
./server 
然后打开一个新的build终端
./client local

服务端也可以采用pyqt5_gui
python start.py local

若想改变默认配置自行修改config内的yaml文件

将本地代码push到jetson-nano: git push nano master
在nano端合并: git reset --hard HEAD
