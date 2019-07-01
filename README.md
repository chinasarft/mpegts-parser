1. 打印原始pts和pcr
2. 检查PAT PMT的crc32
3. 检查pes的长度
4. 检查continuity_counter
5. 打印private stream type的raw descriptor


cmake:
cmake -DCMAKE_INSTALL_LIBDIR=/dir
cmake -DQTDIR=/Users/liuye/Qt5.12.0/5.12.0/clang_64 -DCMAKE_BUILD_TYPE=Debug  -G Xcode ..
