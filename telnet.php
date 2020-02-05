<?php
date_default_timezone_set('Europe/Moscow');
set_time_limit(0);
error_reporting(E_ERROR | E_WARNING | E_PARSE);


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


$records_dir = '/root/.local/share/teeworlds/records/';


if(file_exists($start_file))
	exit;
touch($start_file);



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
	return $data;
}

function send_msg($text)
{
	global $socket;
	$text = 'say "'.$text.'"';
	socket_write($socket, $text."\n", strlen($text)+1);
}
function send_welcome($player_id, $text)
{
	global $socket;
	$text = 'fake 3 '.$player_id.' '.$player_id.' "'.$text.'"';
	#print $text;
	socket_write($socket, $text."\n", strlen($text)+1);
}
function send_cmd($text)
{
	global $socket;
	socket_write($socket, $text."\n", strlen($text)+1);
}

function create_cfg($new_random_map='')
{
	global $maps_dir, $records_dir, $maps_arr, $srv, $db_host, $db_pass, $multi_start_crop_pos, $random_map;


	$maps_limit = 100;

	$maps_temp_arr = preg_grep('/^([^.])/', scandir($maps_dir));
	$maps_arr_bak = $maps_temp_arr;
	unset($maps_temp_arr[array_search('solo',$maps_temp_arr)]);


	$db = new SQLite3(dirname(__FILE__).'/db_records', SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
	$results = $db->query('SELECT * FROM records');
	$vote_finished_maps_arr = array();
	while($arr = $results->fetchArray(SQLITE3_ASSOC))
	{
		$db_player_name = $arr['player_name'];
		$db_map_name = $arr['map_name'];
		$db_map_name = str_replace('_record.dtb', '.map', hex2bin($db_map_name));
		if(in_array($db_map_name, $maps_arr_bak))
		{
			$key = array_search($db_map_name, $maps_temp_arr);
			$vote_finished_maps_arr[$db_map_name] += 1;
			#if(!$solo)
				unset($maps_temp_arr[$key]);
		}

	}
	$db->close();

	
	#if($srv == 'solo')
	#	arsort($vote_finished_maps_arr);
	#else
	#	asort($vote_finished_maps_arr);

	$count_vote_finished_maps_arr = count($vote_finished_maps_arr);


	arsort($vote_finished_maps_arr);

	$vote_finished_maps_arr = array_slice($vote_finished_maps_arr, $multi_start_crop_pos, $maps_limit);


	unset($maps_arr_bak);

	if(!count($maps_arr))
	{

		$maps_arr = $maps_temp_arr;
		
		/*
		if($srv != 'solo')
		{
			$maps_arr_bak = $maps_arr;
			$maps_history = explode('|||', file_get_contents(__DIR__.'/maps_history'));
			foreach($maps_history as $value)
			{
				$key = array_search($value,$maps_arr);
				unset($maps_arr[$key]);
			}
			if(!count($maps_arr))
			{
				$maps_arr = $maps_arr_bak;
				unlink(__DIR__.'/maps_history');
			}
		}
		*/
		
	}
	unset($maps_arr_bak);
	shuffle($maps_arr);
	if(strlen($new_random_map))
		$random_map = str_replace('.map', '', array_shift($maps_arr));

	
	print "random_map:\t\t".$random_map."\n";
	$maps_count = count($maps_arr);

	send_cmd('clear_votes');
	if($srv == 'solo')
		send_cmd('add_vote "[random #'.$maps_count.'] '.$random_map.'" "echo Start_Gen_Random_Map; change_map /solo/'.$random_map.'"');
	else
		send_cmd('add_vote "[random #'.$maps_count.'] '.$random_map.'" "echo Start_Gen_Random_Map; change_map '.$random_map.'"');

	

	$i = $multi_start_crop_pos+1;
	$vote_change_top = $multi_start_crop_pos+$maps_limit;
	if($vote_change_top>$count_vote_finished_maps_arr)
		$vote_change_top = 0;
	send_cmd('add_vote "--- [cfg] Start maplist from #'.($vote_change_top+1).'" "echo ReSlice_TOP_-'.$vote_change_top.'|"');

	foreach($vote_finished_maps_arr as $key=>$value)
	{
		$key = str_replace('.map', '', $key);

		if($srv == 'solo')
			send_cmd('add_vote "[top #'.$i.'] '.$key.'" "change_map /solo/'.$key.'"');
		else
			send_cmd('add_vote "[top #'.$i.'] '.$key.'" "change_map '.$key.'"');
		$i += 1;
	}


}

function player_stat($buf, $parse_from_text='')
{
	global $records_dir, $db_host, $db_pass, $maps_dir;
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
		$msg = 'Player \"'.$player_name.'\" finished ∞ map(s)';
		return $msg;
	}

	if(!strlen($player_name))
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


	$msg = 'Player \"'.$player_name.'\" finished '.$maps_finished_count.' map(s)';
	$player_rank = players_top(bin2hex($player_name));	
	if(strlen($player_rank))
		$msg .='. He is #'.$player_rank.' in top';
	return $msg;
}

function players_top($player_name='', $num_count='')
{
	global $db_host, $db_pass, $maps_dir;
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

	if(strlen($player_name))
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
			$msg = '#'.$i.'. \"'.hex2bin($key).'\" finished '.$value.' map(s)';
			send_msg($msg);
			$i += 1;
		}
		$msg = '------------------------------';
		send_msg($msg);
	}
}

function save_player($buf)
{
	global $db_today;

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
	print $msg;
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

	if(substr($out, 22, 18) == 'player has entered')
	{
		$player_id = bin2hex(parse($out, 'ClientID=', ' addr='));
		if($player_id != '2d31')
		{
			$player_ip = bin2hex(parse($out, 'addr=', ':'));

			$player_nick = explode(':', $out)[1].'-trash';
			$player_nick = parse($player_nick, ' ', "\n");
			$player_nick = bin2hex(trim($player_nick));

			#$msg = 'Welcome '.hex2bin($player_nick).'! ^_^';
			#send_welcome(hex2bin($player_id), $msg);
			
			$db = new SQLite3(dirname(__FILE__).'/'.$db_online, SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
			$db->query('CREATE TABLE IF NOT EXISTS "'.$db_online.'" ("id" VARCHAR NOT NULL, "ip" VARCHAR NOT NULL, "nick" VARCHAR NOT NULL)');
			$db->exec('INSERT INTO "'.$db_online.'" ("id", "ip", "nick") SELECT "'.$player_id.'", "'.$player_ip.'", "'.$player_nick.'" WHERE NOT EXISTS (SELECT id FROM '.$db_online.' WHERE id = "'.$player_id.'");');
			$db->close();
			
			save_player($player_ip);
		}
	}

	if(substr($out, 22, 14) == 'client dropped')
	{
		$player_id = bin2hex(parse($out, 'cid=', ' addr='));
		$db = new SQLite3(dirname(__FILE__).'/'.$db_online, SQLITE3_OPEN_CREATE | SQLITE3_OPEN_READWRITE);
		$db->exec('DELETE FROM '.$db_online.'  WHERE id="'.$player_id.'"');
		$db->close();
	}

	if(substr($out, 23, 13) == 'ReSlice_TOP_-')
	{
		$multi_start_crop_pos = parse($out, 'ReSlice_TOP_-','|');
		create_cfg();
	}

	if(substr($out, 23, 20) == 'Start_Gen_Random_Map')
	{
		#$current_map = parse($out, '[server]: maps/', ' sha256 is ');
		#file_put_contents(__DIR__.'/maps_history', $current_map.'|||', FILE_APPEND);
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
		send_msg('Use \"!top\" instead');
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
	if(crop_buf() == '!server')
	{
		$msg = players_today($out);
		send_msg($msg);
		$f1 = preg_grep('/^([^.])/', scandir($records_dir));
		$f2 = preg_grep('/^([^.])/', scandir($maps_dir));
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
	}
}



?>

