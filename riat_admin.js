// $Id: riat_admin.js,v 1.2 2009/07/29 22:20:36 eclipsegc Exp $

$(document).ready(function() {
  $('span.relationship_options div.options').hide();
  $('span.relationship_options > a').click(function () {
    $(this).siblings('div.options').slideToggle();
  });
});