<?php
date_default_timezone_set('Europe/Moscow');
set_time_limit(0);
error_reporting(E_ERROR | E_WARNING | E_PARSE);


$ver = '0.2.3';
$pass = '12345';

$srv = $argv[1];

$cmd_file = __DIR__.'/cmd';
$start_file = __DIR__.'/server_started';
$db_today = 'db_today';
$db_online = 'db_online';
		
if($srv == 'solo')
{
	$maps_dir = __DIR__.'/data/maps/solo/';
	$port = '8404';

	$cmd_file .= '_solo';
	$start_file .= '_solo';
	$db_today .= '_solo';
	$db_online .= '_solo';
}else{
	$maps_dir = __DIR__.'/data/maps/';
	$port = '8403';

}

$maplist_type = 'unfinished';
$records_dir = '/root/.local/share/teeworlds/records/';


#if(file_exists($start_file))
#	exit;
#touch($start_file);



function parse(&$text,$leftside,$rightside,$last_symbol='')
{
	$left=strpos($text,$leftside);
	if($left=="")
		return -1;
	$left+=strlen($leftside);
	if($last_symbol)
		$right=strrpos($text,$rightside,$left); //последнее вхождение
	else
		$right=strpos($text,$rightside,$left); //первое вхождение
	if($right=="")
		return -1;
	$res=substr($text, $left, $right-$left);
	$text=substr($text,$left+1);
	return $res;
}

function send($url,$post_data="")
{
	global $cookie;
	$headers=array('Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8', 'Cookie: '.$cookie);
	$headers[]='Origin: https://ipdata.co';
	$headers[]='Referer: https://ipdata.co';
	$headers[]='User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36 OPR/57.0.3098.116';
	$headers[]='Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4';
	$headers[]='Connection: keep-alive';

	$ch = curl_init();

	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

	#curl_setopt($ch, CURLOPT_HEADER, 1);
	curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);

	curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER,0);
	curl_setopt($ch, CURLOPT_SSL_VERIFYHOST,0);
	curl_setopt($ch, CURLOPT_CONNECTTIMEOUT,0);

	curl_setopt($ch, CURLOPT_TIMEOUT,4);

	curl_setopt($ch, CURLOPT_ENCODING, "gzip, deflate"); //все поддерживаемые кодировки
	if($post_data)	
	{
		curl_setopt($ch, CURLOPT_POST, 1);
		curl_setopt($ch, CURLOPT_POSTFIELDS, $post_data);
		
	}else{
		curl_setopt($ch, CURLOPT_HTTPGET, 1);
	}
	$data=curl_exec($ch);
	curl_close($ch);
	return $data;
}

function send_msg($text)
{
	global $socket;
	$text = 'say "'.addslashes($text).'"';
	$text .= "\n";
	socket_write($socket, $text, strlen($text));
}
function send_welcome($player_id, $text)
{
	global $socket;
	$text = 'fake 3 '.$player_id.' '.$player_id.' "'.addslashes($text).'"';
	$text .= "\n";
	socket_write($socket, $text, strlen($text));
}
function send_cmd($text)
{
	global $socket;
	$text .= "\n";
	socket_write($socket, $text, strlen($text));
}

function create_cfg($new_random_map='')
{
	global $maps_dir, $records_dir, $maps_arr, $srv, $multi_start_crop_pos, $random_map, $maplist_type, $maps_count;


	$maps_limit = 100;

	$maps_temp_arr = preg_grep('/^([^.])/', scandir($maps_dir));
	$maps_arr_bak = $maps_temp_arr;
	unset($maps_temp_arr[array_search('solo',$maps_temp_arr)]);


	$db = new SQLite3(dirname(__FILE__).'/db_records', SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);	
	$vote_finished_maps_arr = array();
	if($maplist_type == 'unfinished')
	{
		foreach($maps_arr_bak as $map_name)
		{

			$temp_map_name = bin2hex(str_replace('.map', '_record.dtb', $map_name));
			$db_res = $db->querySingle('SELECT DISTINCT "map_name" FROM "records" WHERE map_name = "'.$temp_map_name.'"', true);
			if(!count($db_res))
			{
				$key = array_search($map_name, $maps_temp_arr);
				$vote_finished_maps_arr[$map_name] += 1;
				if(is_int($key))
				{
					unset($maps_temp_arr[$key]);
				}
			}

		}
	}else{
		$results = $db->query('SELECT "map_name" FROM "records"');
		while($arr = $results->fetchArray(SQLITE3_ASSOC))
		{
			$db_player_name = $arr['player_name'];
			$db_map_name = $arr['map_name'];
			$db_map_name = str_replace('_record.dtb', '.map', hex2bin($db_map_name));
			if(in_array($db_map_name, $maps_arr_bak))
			{

				$key = array_search($db_map_name, $maps_temp_arr);
				$vote_finished_maps_arr[$db_map_name] += 1;
				if(is_int($key))
					unset($maps_temp_arr[$key]);
			}
		}
	}


	$db->close();
	unset($maps_arr_bak);


	$count_vote_finished_maps_arr = count($vote_finished_maps_arr);
	if($maplist_type == 'unfinished')
		ksort($vote_finished_maps_arr);
	else
		arsort($vote_finished_maps_arr);
	$vote_finished_maps_arr = array_slice($vote_finished_maps_arr, $multi_start_crop_pos, $maps_limit);


	if(!count($maps_arr))
		$maps_arr = $maps_temp_arr;
	unset($maps_temp_arr);

	shuffle($maps_arr);
	
	if(strlen($new_random_map))
	{
		$maps_count = count($maps_arr);
		$random_map = str_replace('.map', '', array_shift($maps_arr));
		print "random_map:\t\t".$random_map."\n";
	}	

	if($maplist_type == 'unfinished')
	{
		$rand_str = 'finished';
		$vote_str = 'unfinished';
	}else{
		$rand_str = 'unfinished';
		$vote_str = 'top';
	}

	send_cmd('clear_votes');


	if($maplist_type == 'unfinished')
		$vote_maplist_type = 'finished';
	else
		$vote_maplist_type = 'unfinished';
	send_cmd('add_vote "[cfg] Show maplist for '.$vote_maplist_type.' maps" "echo Change_Maplist_Type_-'.$vote_maplist_type.'|"');

	$i = $multi_start_crop_pos+1;
	$vote_change_top = $multi_start_crop_pos+$maps_limit;
	if($vote_change_top>$count_vote_finished_maps_arr)
		$vote_change_top = 0;
	send_cmd('add_vote "[cfg] Show maplist from #'.($vote_change_top+1).'" "echo ReSlice_TOP_-'.$vote_change_top.'|"');

	send_cmd('add_vote "----------------" "echo line"');

	if($srv == 'solo')
		send_cmd('add_vote "['.$rand_str.' #'.$maps_count.'] '.$random_map.'" "echo Start_Gen_Random_Map; change_map /solo/'.$random_map.'"');
	else
		send_cmd('add_vote "['.$rand_str.' #'.$maps_count.'] '.$random_map.'" "echo Start_Gen_Random_Map; change_map '.$random_map.'"');

	send_cmd('add_vote "---------------" "echo line"');

	foreach($vote_finished_maps_arr as $key=>$value)
	{
		$key = str_replace('.map', '', $key);

		if($srv == 'solo')
			send_cmd('add_vote "['.$vote_str.' #'.$i.'] '.$key.'" "change_map /solo/'.$key.'"');
		else
			send_cmd('add_vote "['.$vote_str.' #'.$i.'] '.$key.'" "change_map '.$key.'"');
		$i += 1;
	}


}

function player_stat($buf, $parse_from_text='')
{
	global $records_dir, $maps_dir;
	$curr_maps_arr = preg_grep('/^([^.])/', scandir($maps_dir));
	if($parse_from_text)
	{
		$player_name = explode(':', $buf)[6];
		$player_name = str_replace(' !stat ', '', $player_name);
		$player_name = trim($player_name);
	}else{
		$player_name = explode(':', $buf)[5];
	}

	if($player_name=='blin4ik' or $player_name=='run')
	{
		$msg = 'Player "'.$player_name.'" finished ∞ map(s)';
		return $msg;
	}

	if(!mb_strlen($player_name,'UTF-8') or mb_strlen($player_name,'UTF-8')>32)
	{
		$msg = 'Bad nickname :(';
		return $msg;
	}

	$db = new SQLite3(dirname(__FILE__).'/db_records', SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
	$results = $db->query('SELECT map_name FROM records WHERE player_name = "'.bin2hex($player_name).'"');

	$maps_finished_count = 0;

	while($arr = $results->fetchArray(SQLITE3_ASSOC))
	{
		$db_map_name = $arr['map_name'];	
		$db_map_name = str_replace('_record.dtb', '.map', hex2bin($db_map_name));
		if(in_array($db_map_name, $curr_maps_arr))
		{
			$maps_finished_count += 1;
		}else{
		}

	}
	$db->close();


	$msg = 'Player "'.$player_name.'" finished '.$maps_finished_count.' map(s)';
	$player_rank = players_top(bin2hex($player_name));	
	if(strlen($player_rank))
		$msg .='. He is #'.$player_rank.' in top';
	return $msg;
}

function players_top($player_name='', $num_count='')
{
	global $maps_dir;
	$curr_maps_arr = preg_grep('/^([^.])/', scandir($maps_dir));
	$db = new SQLite3(dirname(__FILE__).'/db_records', SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
	$results = $db->query('SELECT * FROM records');
	$top = array();	
	while($arr = $results->fetchArray(SQLITE3_ASSOC))
	{

		$db_player_name = $arr['player_name'];
		$db_map_name = $arr['map_name'];	
		$db_map_name = str_replace('_record.dtb', '.map', hex2bin($db_map_name));
		if(in_array($db_map_name, $curr_maps_arr))
		{
			$top[$db_player_name] += 1;
		}

	}
	$db->close();
	arsort($top);

	if(mb_strlen($player_name,'UTF-8'))
	{
		$player_rank = array_search($player_name, array_keys($top));
		if(strlen($player_rank))
			$player_rank += 1;
		return $player_rank;
	}else{
		$i = 1;
		$msg = '----------- Top '.$num_count.' -----------';
		send_msg($msg);
		foreach($top as $key=>$value)
		{
			if($i>$num_count)
				break;
			$msg = '#'.$i.'. "'.hex2bin($key).'" finished '.$value.' map(s)';
			send_msg($msg);
			$i += 1;
		}
		$msg = '------------------------------';
		send_msg($msg);
	}
}

function save_player($buf)
{
	global $db_today, $last_clear;

	if($last_clear != date('d', time()))
	{
		$last_clear = date('d', time());
		unlink(__DIR__.'/'.$db_today);
	}

	$db = new SQLite3(dirname(__FILE__).'/'.$db_today, SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
	$db->query('CREATE TABLE IF NOT EXISTS "'.$db_today.'" ("player_ip" VARCHAR NOT NULL)');

	$db_res = $db->querySingle('SELECT "player_ip" FROM "'.$db_today.'" WHERE player_ip = "'.$buf.'"', true);
	if(!count($db_res))
			$db->exec('INSERT INTO "'.$db_today.'" ("player_ip") VALUES ("'.$buf.'")');
	$db->close();
}

function players_today($buf)
{
	global $db_today;

	$db = new SQLite3(dirname(__FILE__).'/'.$db_today, SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
	$today = $db->querySingle('SELECT COUNT(*) FROM '.$db_today);
	$msg = 'Today '.$today.' people played on the server';
	$db->close();
	return $msg;
}

function crop_buf()
{
	global $out;
	$crop = explode(':', $out)[6];
	$crop = trim($crop);

	return $crop;
}



ob_implicit_flush();

function connect_telnet()
{
	global $socket, $port;
	while(true)
	{
		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		$result = socket_connect($socket, '127.0.0.1', $port);
		if(!$result)
		{
			socket_clear_error($socket);
			socket_close($socket);
			print "repeat\n";
			sleep(1);
		}else{
			break;
		}
	}
}


$multi_start_crop_pos=0;

while(true)
{
	$out = socket_read($socket, 512, PHP_NORMAL_READ);
	if(!$out)
	{
		socket_clear_error($socket);
		socket_close($socket);
		connect_telnet();
		continue;
	}
	if($out == "Enter password:\n")
	{
		send_cmd($pass);
		#sleep(1);
		create_cfg('gen_random_map');
	}

	if(file_exists($cmd_file))
	{
		send_cmd(file_get_contents($cmd_file));
		unlink($cmd_file);
	}


	#print "--=".$out."=--";

	if(substr($out, 12, 28) == '[server]: player has entered')
	{
		$player_id = bin2hex(parse($out, 'ClientID=', ' addr='));
		if($player_id != '2d31')
		{
			$player_ip = bin2hex(parse($out, 'addr=', ':'));

			$player_nick = explode(':', $out)[1].'-trash';
			$player_nick = parse($player_nick, ' ', "\n");
			$player_nick = bin2hex(trim($player_nick));

			
			$db = new SQLite3(dirname(__FILE__).'/'.$db_online, SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
			$db->query('CREATE TABLE IF NOT EXISTS "'.$db_online.'" ("id" VARCHAR NOT NULL, "ip" VARCHAR NOT NULL, "nick" VARCHAR NOT NULL)');
			$db->exec('INSERT INTO "'.$db_online.'" ("id", "ip", "nick") SELECT "'.$player_id.'", "'.$player_ip.'", "'.$player_nick.'" WHERE NOT EXISTS (SELECT id FROM '.$db_online.' WHERE id = "'.$player_id.'");');
			$db->close();
			
			save_player($player_ip);
		}
	}

	if(substr($out, 12, 24) == '[server]: client dropped')
	{
		$player_id = bin2hex(parse($out, 'cid=', ' addr='));
		$db = new SQLite3(dirname(__FILE__).'/'.$db_online, SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
		$db->exec('DELETE FROM '.$db_online.'  WHERE id="'.$player_id.'"');
		$db->close();
	}

	if(substr($out, 12, 23) == '[Console]: ReSlice_TOP_')
	{
		$multi_start_crop_pos = parse($out, 'ReSlice_TOP_-','|');
		create_cfg();
	}

	if(substr($out, 12, 32) == '[Console]: Change_Maplist_Type_-')
	{
		$maplist_type = parse($out, 'Change_Maplist_Type_-','|');
		unset($maps_arr);
		$multi_start_crop_pos = 0;
		create_cfg('gen_random_map');
	}

	if(substr($out, 12, 31) == '[Console]: Start_Gen_Random_Map')
	{
		create_cfg('gen_random_map');
	}


	if(crop_buf() == '!time')
	{
		$player_id = bin2hex(parse($out, '][chat]: ', ':'));

		$db = new SQLite3(dirname(__FILE__).'/'.$db_online, SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
		$db_res = $db->querySingle('SELECT "ip" FROM "'.$db_online.'" WHERE "id" = "'.$player_id.'"', true);
		$db->close();

		$ipinfo = send('https://api.ipdata.co/'.hex2bin($db_res['ip']).'/time_zone/?api-key=9b8a49e10bc989a900a2d1052999045d9450c3cee479de571ac81d9e');
		$offset = parse($ipinfo, '"offset": "', '"');
		$offset = str_replace('+0', '+', $offset);
		$offset = rtrim($offset, '0');
		$offset = 'UTC'.$offset;
		$time = parse($ipinfo, '"current_time": "', '.');

		$date = date_create($time);
		$time = date_format($date, 'H:i');

		$msg = 'Now: '.$time.' ('.$offset.')';
		if($offset == '-1' or $time == '-1')
			$msg = 'Something wrong';
		send_msg($msg);
	}

	if(crop_buf() == '!online')
	{
		$db = new SQLite3(dirname(__FILE__).'/'.$db_online, SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
		$count_online = $db->querySingle('SELECT COUNT(*) FROM '.$db_online);
		$db->close();
		$msg = 'Now '.$count_online.' player(s) online';
		send_msg($msg);
	}
	if(crop_buf() == '!stat_all')
	{
		send_msg('Use "!top" instead');
	}
	if(crop_buf() == '!top' or crop_buf() == '!top5')
	{
		players_top('', 5);
	}
	if(crop_buf() == '!top10')
	{
		players_top('', 10);
	}
	if(crop_buf() == '!stat')
	{
		$msg = player_stat($out);
		send_msg($msg);
	}
	if(substr(crop_buf(), 0, 6) == '!stat ')
	{
		$msg = player_stat($out, 'parse');
		send_msg($msg);
	}
	if(crop_buf() == '!server' or crop_buf() == '!info')
	{
		$msg = '-- TeeRace econ v'.$ver;
		send_msg($msg);

		$msg = players_today($out);
		send_msg($msg);

		$f1 = preg_grep('/^([^.])/', scandir($records_dir));
		$f2 = preg_grep('/^([^.])/', scandir($maps_dir));
		unset($f2[array_search('solo',$f2)]);
		$finished_maps_count = 0;
		foreach($f1 as $rec_v)
		{
			$rec_v = str_replace('_record.dtb', '.map', $rec_v);
			if(in_array($rec_v, $f2))
			{
				$finished_maps_count += 1;
			}
		}
		$msg = $finished_maps_count.' of '.count($f2).' maps completed by players';
		send_msg($msg);

		$msg = '-- https://github.com/oprw/teerace';
		send_msg($msg);
	}
}



?>

