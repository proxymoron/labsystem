<?php
require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );
require_once( INCLUDE_DIR."/config.inc" );
require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );

class MailFunctionality{
	/*
	 * For debugging purposes you can use this function to display a mail instead of sending it out.
	 */
	function printMail($to, $subject, $message, $additional_headers='', $additional_parameters=''){
		echo('<br>'.PHP_EOL.'0-0-0-0-0-0-0-0-0'.'<br>'.PHP_EOL);
		echo('To: '.$to.'<br>'.PHP_EOL);
		echo('Subject: '.$subject.'<br>'.PHP_EOL);
		echo('Message: '.$message.'<br>'.PHP_EOL);
		echo('Additional Headers: '.$additional_headers.'<br>'.PHP_EOL);
		echo('Additional Parameters: '.$additional_parameters.'<br>'.PHP_EOL);
		echo('<br>'.PHP_EOL.'0-0-0-0-0-0-0-0-0'.'<br>'.PHP_EOL);
	}
	
   /**
   * Send a mail.
   * @param String $sender              The sender.
   * @param String $receiverUIDs        The comma separated list of receiver UIDs.
   * @param String $subject             The mail subject.
   * @param String $text                The mail text.
   * @param Boolean $CCsender            Send mail via CC to sender?
   * @param Boolean $BCC                 Send mail via BCC?
   */
  function sendMail( $sender, $receiverUIDs, $subject, $text, $CCsender=FALSE, $BCC=FALSE, $additionalHeaders='' ){
    global $cfg, $usr, $uDBI, $SYSTEMMAIL_SENDER, $urDBI;
    
    if (empty($sender)){
    	$sender = '<noreply@'.$_SERVER['SERVER_NAME'].'>';
    }
    
    preg_match("/\<(.*)\>/", $sender, $output_array);
    $senderMail = (!empty($output_array) ? $output_array[1] : $sender);
    if (empty($senderMail)){
    	$senderMail = $sender;
    }
    
    $receiver = '';
    $allReceivers = ''; // Collect receivers for summary report to sender
    if ($CCsender){
    	$receiverUIDs .= ', labsystemReportSender';
    }
    
    foreach( explode(',', $receiverUIDs) as $receiverUID ){
    	$receiverUID=trim($receiverUID);
	    // Load receiver data including mail address
	    if ($receiverUID != 'labsystemReportSender'){
	    	$receiverUsrDataArray = $uDBI->getData4( $receiverUID );
	    	$userDetailedData = $urDBI->getData4( $receiverUID );
	    	$receiverUsrObject = new ElementUser( 
	    			$receiverUID,//$uid
	    			$receiverUsrDataArray['userName'], //$userName
	    			$receiverUsrDataArray['foreName'], //$foreName
	    			$receiverUsrDataArray['name'], //$surName
	    			$userDetailedData['currentTeam'],//$currentTeam
	    			0,//$userRights
	    			0,//$userMaximumRights
	    			$receiverUsrDataArray['eMail'],//$mailAddress
	    			'created just for mail');
	    	// replace constants for personalized mails
	    	$replacedSubject = $this->replaceConstants($subject, $receiverUsrObject);
	    	$receiver = $receiverUsrObject->mailAddress;
	    	$allReceivers .= '"'.$receiverUsrObject->foreName.' '.$receiverUsrObject->surName.' ('.$receiverUsrObject->currentTeam.')" <'.$receiverUsrObject->mailAddress.'>'.PHP_EOL;
	    	$replacedText = $this->replaceConstants($text, $receiverUsrObject);
	    } else {
	    	// Report for sender:
	    	$replacedSubject = $subject.' (report)';
	    	$receiver = $senderMail;
	    	$replacedText = 'Sent to:'.PHP_EOL.$allReceivers.'--'.PHP_EOL.PHP_EOL.$text;
	    }
	    
	    //$this->printMail 
	    mail( ($BCC ? $senderMail : $receiver), // TO
	          '['.$cfg->get("SystemTitle").'] '.$replacedSubject,
	          $replacedText.PHP_EOL.PHP_EOL.
	          stripcslashes($cfg->get('mailFooter')).PHP_EOL,
// 	          ($CCsender ? 'CC: '.$senderMail.PHP_EOL:''). // CC
	          ($BCC ? 'BCC: '.$receiver.PHP_EOL : ''). // BCC
	          'From: '.$cfg->get('SystemTitle').' <'.$SYSTEMMAIL_SENDER.'>'.PHP_EOL. /* The labsystem is always sending mails using the configured system mail address */
	    	  'Reply-To: '.$sender.PHP_EOL.
	          "X-Mailer: labsystem".PHP_EOL.
	          (!empty($additionalHeaders)&&(strpos(strtoupper($additionalHeaders),'CONTENT-TYPE:') === false)?'Content-Type: text/plain; charset=UTF-8'.PHP_EOL:'').
	          stripcslashes($additionalHeaders).
	          'X-Sending-Username: '.$usr->userName.'@'.$cfg->get("SystemTitle").PHP_EOL. // this is for identifying a user (username must be correct...)
	          stripcslashes($cfg->get('mailHeaderAdd'))
	    );
     }
  }
  
  /**
   * Replaces the page constants in the mail text with the current values.
   * This is valuable for personalized or othewise customized mails.
   * @param $text The text the constants should be replaced within.
   * @param $currentUsrObject The user object to be used for replacing the constants, e.g. user name.
   */
  function replaceConstants( $text, $currentUsrObject ){
  	global $usr, $pge;
  	$tempUsr = $GLOBALS['usr'];
  	$GLOBALS['usr'] = $currentUsrObject;
  	if ( isset($pge) ){
  		$replacedText = $pge->replaceConstantsNonHTML( $text );
  	}else{
  		$replacedText = $text;
  	}
  	$GLOBALS['usr'] = $tempUsr;
  	return $replacedText;
  }

  /**
   * Sends a mail to all users that belong to all groups given in the $accessRightsMask.
   * @param Integer $accessRightsMask   The mask of access rights that qualify the receiver.
   * @param String $subject             The mail subject.
   * @param String $text                The mail text.
   */
  function mail2groupWithRights( $accessRightsMask, $subject, $text ){
    global $urDBI;
    $urDBI->getAllData();
    // iterate over all users that have rights
    $receivers = array();
    while( $userRightsData = $urDBI->getNextData() ){
      if (ElementUser::isOfKind($accessRightsMask, $userRightsData["rights"])) {
        // aggregate users first:
      	array_push( $receivers, $userRightsData['uid'] );
      }
    }
    // Send mass mail:
    $this->sendMail('', implode(',', $receivers), $subject, $text);
  }
}
$mailFunc = new MailFunctionality();
?>
