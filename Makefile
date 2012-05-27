all:
	g++ -O2 *.cpp -o ptcloud_world
	g++ -DCONF_ -O2 *.cpp -o ptcloud_conf
