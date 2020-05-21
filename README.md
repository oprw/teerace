# Deploy:
1. build server (directory 'teerace')
1. modify 'cfg_', 'cfg_solo'
1. put directory 'data' *(it contain a folder with maps\etc)* to the same directory where located compiled server
1. put directory 'records' *(it contain a saved records for stat)* in *home directory*/.local/share/teeworlds/
1. run server with argument 'exec cfg_' *(for main server)* or 'exec cfg_solo' *(for solo server)*, put it in cron\systemd\etc
1. apt install *(or your package manager)* php-cli php-curl php-sqlite3 php-mbstring
1. edit, run, 'monitor_records.php', put in cron\systemd\etc
1. put 'telnet.php' to the same directory where located compiled server, edit, run 'telnet.php' without arguments (for main server) or with 'solo' (for solo server), put it in cron\systemd\etc
1. **Congratulations! Now you have full copies of both servers**

# Pancakes for everyone (ʘ‿ʘ)
