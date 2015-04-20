mkdir build
cd build
cmake -DPLUGIN_DIR="/opt/bartlby-agent/plugins/" -DBARTLBY_USER="travis" -DMYSQL_HOST="localhost" -DMYSQL_USER=root -DMYSQL_PASS="" -DMYSQL_DB=bartlby -DCMAKE_INSTALL_PREFIX:PATH=$PWD/installation -DFULL_FEATURES=1 ..
make
make install
ls -l $PWD/installation
mysql -e 'create database bartlby;'
cd $PWD/installation
mysql bartlby  < $PWD/mysql.shema
#bin/bartlby -d -r $PWD/etc/bartlby.cfg 2>&1 >> $PWD/run.log &
$PWD/etc/bartlby.startup start

exit 0;







