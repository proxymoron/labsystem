<?php
/**
 *  labsystem.m-o-p.de -
 *                  the web based eLearning tool for practical exercises
 *  Copyright (C) 2010  Marc-Oliver Pahl
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
* Contains the Pge class.
*/

/**
* This class encapsulates the current Page (the one that will be diplayed).
* This class extends the elements/LpElement.inc.
*
* An instance of this class gets created at the end: $pge.
*
* @module     ../include/classes/ElementMenues.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
class Pge extends LpElement{
  var $nextTab = 1; // the value of the next available TabNumber (for forms and menu elements)
  var $replaceKey;

  /**
  * In this function the strings that will be replaced when displaying and their replacements are initialized.
  */
  function Pge(){
    global $cfg, $usr;
    $this->LpElement( 0, "?untitled page!", "", "", 0, false, "" );
    $this->replaceKey   = Array( "__LINKQUERY__",
                                 "__PAGETITLE__",
                                 "__SYSTEMTITLE__",
                                 "__FORENAME__",
                                 "__NAME__",
                                 "__USERNAME__",
                                 "__TEAMNUMBER__",
                                 "__MAILADDRESS__",
                                 "__SERVERNAME__",
                                 "__ELEMENTTITLE__",
                                 "__CURRENT_URL__",
                                 "__LINK2ADMIN_USER_INTERFACE__",
                                 "__CURRENT_TIMESTAMP__" );
    $this->sanitizedKey   = Array( "awob6ope69",
                                "uhuk8afa95",
                                "uvur2awu94",
                                "onot5oka70",
                                "elid5upe10",
                                "anus9iri39",
                                "avod1ubo72",
                                "bW3Bsd8JlC",
                                "ubo72avod1",
                                "ugud9eka32",
                                "eka32ugud9",
                                "udan1obo94",
                                "siducb4uib" );
  }

  /**
  * Override to the Element::isVisible();
  * Is the element visible?
  * Necessary because IamVisible is not set correctly! (LpElement on construction)
  * @return BOOL  Is the element visible?
  */
  function isVisible(){
    global $usr;
    $this->IamVisible = ( ($this->visibleFor == "0") || $usr->isOfKind( $this->visibleFor ) );
    return parent::isVisible();
  }

  /**
  * Adds (puts) content to the page.
  * @param $string  The content to be added.
  */
  function put( $string ) {
    $this->contents .= $string;
  }

  /**
  * In some markup languages the chosen constants $this->replaceKey have markup meaning.
  * Replaces the  $this->replaceKey by  $this->sanitizedKey which should be save.
  * Became necessary with the introduction of MarkUp as they use __ as mark for emphasis.
  * @param $string  The content the replacements should be done inside.
  * @return $string  The content with the replacements done.
  */
  function sanitizeKeys( $string ){
    return str_replace( $this->replaceKey, $this->sanitizedKey, $string );
  }

  function escapeKeys( $string ){
    return str_replace( '_', '&#x005F;', $string );
  }


  /**
   * Replaces the constants with the values but does NOT htmlexcape the values. 
   * This allows crossitescripting, w.g. by replacing my user name with JS code. 
   * However, it only works for loged in users.
   * 
   * This is used in the eMail functionality.
   * 
   * @param String The content the constants should be replaced in.
   * @return The string with the replaced constants.
   */
  function replaceConstantsNonHTML( $text ){
  	global $url, $usr, $lng, $cfg;
  	$replaceValue = Array( $url->get('newQueryString'), // This might change during the generation and should be replaced last...
  			$this->title,
  			$cfg->get("SystemTitle"),
  			$usr->foreName,
  			$usr->surName,
  			$usr->userName,
  			$usr->currentTeam,
  			$usr->mailAddress,
  			$_SERVER['SERVER_NAME'],
  			$this->replaceKey[9],
  			$_SERVER['REQUEST_URI'],
  			$cfg->get("Link2UserAdminInterface"),
  			date( $lng->get("DateFormat") ) );
  
  	return str_replace( array_merge( $this->replaceKey, $this->sanitizedKey ), // replace keys and sanitized keys.
  			array_merge( $replaceValue, $replaceValue ),
  			$text );
  }
  
  /**
   * Returns the $content with __ELEMENTTITLE__ replacesd by $elementTitle.
   * @param unknown $title The title.
   */
  function replaceElementTitle( $content, $elementTitle ){
  	$needle = array( $this->replaceKey[9], $this->sanitizedKey[9]);
  	$replace = array($elementTitle, $elementTitle);
  	return str_replace( $needle, $replace, $content );
  }
  
  /**
  * Replaces the constants with the values.
  * @param String The content the constants should be replaced in.
  * @return The string with the replaced constants.
  */
  function replaceConstants( $text ){
    global $url, $usr, $lng, $cfg;
    	$replaceValue = Array( $url->get('newQueryString'), // This might change during the generation and should be replaced last...,
    	                             htmlentities( $this->title ),
    	                             htmlentities( $cfg->get("SystemTitle") ),
    	                             htmlentities( $usr->foreName ),
    	                             htmlentities( $usr->surName ),
    	                             htmlentities( $usr->userName ),
    	                             htmlentities( $usr->currentTeam ),
    	                             htmlentities( $usr->mailAddress ),
    	                             $_SERVER['SERVER_NAME'],
    	                             $this->elementTitle,
    	                             urlencode( $_SERVER['REQUEST_URI'] ),
    	                             $cfg->get("Link2UserAdminInterface"),
    	                             htmlentities( date( $lng->get("DateFormat") ) ) );

    return str_replace( array_merge( $this->replaceKey, $this->sanitizedKey ), // replace keys and sanitized keys.
                        array_merge( $replaceValue, $replaceValue ),
                        $text );
  }

  /**
  * This function is called to display the contents. It gets called in ../include/page_layout.inc.
  * This function overrides the one from LpElement.
  */
  function show(){
    global $lng;
    if ( !$this->isVisible() ) // not enough rights->no content
      $this->contents = "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";

    // The __LINKQUERY__ replacements must be set here since they may not be correct before.
    echo( $this->replaceConstants( $this->contents ) );
  }

  /**
  * This function returns the predefined constants that will be replaced when showing (p.e. __LINKQUERY__).
  * It gets called when one edits a page element.
  */
  function getConstants(){
    $ret = "";
    foreach( $this->replaceKey as $value ) $ret .= ", ".$value;
    return substr( $ret, 2 );
  }
}
$pge = new Pge(); /** create instance */
?>
