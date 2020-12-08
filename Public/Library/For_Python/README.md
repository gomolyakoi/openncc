1 support over python3.5

2 python interface file openncc.py£¬python run shared file _openncc.so

3 example file hello_ncc.py

4 run method: 
1)copy run relation files from how_to_use_sdk to current:
cp -a ../how_to_use_sdk/bin/blob .
cp -a ../how_to_use_sdk/bin/fw .

2) run test file
sudo python3 hello_ncc.py

and also we support human_pose_estimation example,just print the output data from ai module
when run : sudo python3 hello_human.py
