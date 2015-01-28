./autogen.sh
./configure --enable-ssh  --enable-nrpe --enable-ssl --enable-lua --prefix=$PWD/installation --with-user=travis
make
make install
ls -l $PWD/installation
mysql -e 'create database bartlby;'
cd $PWD/installation
mysql bartlby  < $PWD/mysql.shema
#bin/bartlby -d -r $PWD/etc/bartlby.cfg 2>&1 >> $PWD/run.log &
$PWD/etc/bartlby.startup start

exit 0;







