see www.bartlby.org for a wiki

quick INSTALL:
	bartlby-core/
			
			useradd bartlby
			
			./configure --prefix=/usr/local/bartlby \
			--with-mysql-user=bartlby \
			--with-mysql-password=bartlby \
			--with-mysql-host=localhost \
			--with-mysql-db=bartlby \
			--with-plugin-dir=/usr/local/bartlby/plugins/ \
			--with-nrpe=yes \ #optional
			--with-ssl=yes \  #optional
			--with-snmp=yes   #optional
			make
			make install
			
			mysql -u bartlby --password=bartlby bartlby < mysql.shema
			
			/usr/local/bartlby/etc/bartlby.startup remove
			/usr/local/bartlby/etc/bartlby.startup start
			/usr/local/bartlby/etc/bartlby.startup status
			tail -f /usr/local/bartlby/var/log/bartlby.YYYY.MM.DD
			
	bartlby-extensions
			make all
			mkdir /usr/local/bartlby/ext
			cp *.so /usr/local/bartlby/ext/
	bartlby-agent/
			./configure
			make
			make install
			register port 9030 in /etc/services as "bartlbya"
				echo "bartlbya		9030/tcp" >> /etc/services

			inetd:
				echo "bartlbya                stream  tcp     nowait.500      <BARTLBY_USER>  /usr/local/bartlby-agent/bartlby_agent /usr/local/bartlby-agent/bartlby.cfg" >> /etc/inetd.conf

			xinetd:
				vi /etc/xinetd.d/bartlbya
					service bartlbya
					{
        					disable                 = no
					        port                    = 9030
					        socket_type             = stream
					        protocol                = tcp
					        wait                    = no
					        user                    = root
					        server                  = /usr/local/bartlby-agent/bartlby_agent
					        server_args             = /usr/local/bartlby-agent/bartlby.cfg
					}	

			restart either inetd or xinetd
	bartlby-php/
			phpize
			./configure
			make
			make install
			
			echo "<? \
			dl("bartlby.so"); \
			echo confirm_bartlby_compiled("bartlby"); \
			?>"|php
			
	bartlby-ui/
		vi config.php
			$Bartlby_CONF="/usr/local/bartlby/bartlby.cfg";	
		cp -va * /var/www/htdocs/bartlby-ui/
	
	bartlby-plugins/
		mkdir /usr/local/bartlby/plugins/
		make all
		cp -va * /usr/local/bartlby/plugins/
		
SURF to http://YOURHOST/bartlby-ui
for more detailed instructions fixes and work arounds refer to http://www.bartlby.org/
			
