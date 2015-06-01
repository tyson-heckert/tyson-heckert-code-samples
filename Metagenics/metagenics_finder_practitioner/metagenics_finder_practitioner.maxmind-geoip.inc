<?php

function maxmind_geoip(){

	$json_url = 'https://geoip.maxmind.com/geoip/v2.0/city/'.$_SERVER['REMOTE_ADDR'];
	$username = '67879';  // authentication
	$password = 'cM3zaKDPVi3L';  // authentication
	 
	// Initializing curl
	$ch = curl_init( $json_url );
	 
	// Configuring curl options
	$options = array(
	CURLOPT_RETURNTRANSFER => 1,
	CURLOPT_USERPWD => $username . ":" . $password,   // authentication
	CURLOPT_HTTPHEADER => array('
		Content-Type: application/vnd.maxmind.com-city+json; charset=UTF-8; version=2.0') ,
	CURLOPT_SSL_VERIFYPEER => false,
	);
	 
	// Setting curl options
	curl_setopt_array( $ch, $options );
	 
	// Getting results
	$result =  curl_exec($ch);
	curl_close($ch);

	$location = json_decode($result, true);

	return $location;
}