# Deploy:
1. build server (directory 'teerace')
1. modify 'cfg_', 'cfg_solo'
1. put directory 'data' (it contain a folder with maps\etc) to the same directory where located server
1. run server with argument 'exec cfg_' or 'exec cfg_solo' (or run two servers with both arguments), put it in cron\systemd\etc
1. apt install *(or your package manager)* php-cli php-curl php-sqlite3
1. rebuild database for '!stat', '!top' etc (edit, run, put in cron\systemd\etc 'monitor_records.php') (all saved records in directory 'records')
1. edit, run 'telnet.php' without arguments or with 'solo' (or run two copies without and with argument), put it in cron\systemd\etc
1. **Congratulations! Now you have full copies of both servers**

# Pancakes for everyone (ʘ‿ʘ)
