<?php
date_default_timezone_set('Europe/Moscow');
set_time_limit(0);
error_reporting(E_ERROR | E_WARNING | E_PARSE);


$records_dir = '/root/.local/share/teeworlds/records/';

while(true)
{
	$db = new SQLite3(dirname(__FILE__).'/db_records', SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
	$db->query('CREATE TABLE IF NOT EXISTS "records" ("map_name" VARCHAR NOT NULL, "player_name" VARCHAR NOT NULL)');

	$files = preg_grep('/^([^.])/', scandir($records_dir));
	foreach($files as $file)
	{
		$map_name = str_replace('.map', '', $file);
		$map_name = bin2hex($map_name);

		$file = file($records_dir.'/'.$file, FILE_IGNORE_NEW_LINES);

		for($x=0,$l=sizeof($file);$x<$l;$x+=3)
		{
			$player_name = bin2hex($file[$x]);
			if(!strlen($player_name))
				continue;
			$db_res = $db->querySingle('SELECT "map_name" FROM "records" WHERE "map_name" = "'.$map_name.'" and player_name = "'.$player_name.'"', true);

			if(!count($db_res))
				$db->exec('INSERT INTO "records" ("map_name", "player_name") VALUES ("'.$map_name.'", "'.$player_name.'")');
		}
	}
	$db->close();
	sleep(60);
}


?>
