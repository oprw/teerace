# Deploy:
1. build server (dir 'teerace')
1. rebuild database for '!stat', '!top' etc (edit, run, put in cron\systemd\etc 'monitor_records.php') (all saved records in dir 'records')
1. modify 'cfg_', 'cfg_solo'
1. run server with argument 'exec cfg_' or 'exec cfg_solo' (or run two servers with both arguments), put it in cron\systemd\etc
1. edit, run 'telnet.php' without arguments or with 'solo' (or run two copies without and with argument), put it in cron\systemd\etc
1. **Congratulations! Now you have full copies of both servers**

# Pancakes for everyone (ʘ‿ʘ)
