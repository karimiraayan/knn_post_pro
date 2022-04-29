echo "[Init-Script] start"
# in dir: $PWD"
echo "empty - test"

#
# before loading data to mm this script is called (first action of simulator)
# can be used to convert data to binary ...
#
# e.g.:
python3 ../init/img2bin.py ../data/depth2dImage.png 1024 64 ../data/depth2d 2


echo "[Init-Script] done"
