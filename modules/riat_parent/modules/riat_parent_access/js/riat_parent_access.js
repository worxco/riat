// $Id: riat_parent_access.js,v 1.2 2009/08/03 13:32:13 eclipsegc Exp $

$(document).ready(function() {
  var base = $('#edit-base-path').val();
  $('#user_access_table').load(base + 'riat/users');
  $('#user_access_table').html();
  $('#edit-add-button').click(function() {
    var hash = $('#edit-riat-hash').val();
    var user = $('#edit-add').val();
    $.ajax({
      type: "GET",
      url: base + "riat/user_autocomplete_save",
      data: "user=" + user +"&hash=" + hash
    });
    $('#edit-add').val('');
    $('#user_access_table').load(base + 'riat/users');
    return false;
  });
  $('div.riat_user_delete').click(function () {
    return false;
  });
  var label = $('div#riat_access_settings label span.form-required').text();
  if (label == '*') {
    $('#edit-add-wrapper').parent().hide();
  }
  if ($('input#edit-rid-access-settings-public').attr('checked')) {
    $('#edit-add-wrapper').parent().hide();
  }
  if ($('input#edit-rid-access-settings-private').attr('checked')) {
    $('#edit-add-wrapper').parent().show();
  }
  $('input#edit-rid-access-settings-private').click(function () {
    $('#edit-add-wrapper').parent().show('slow');
  });
  $('input#edit-rid-access-settings-public').click(function () {
    $('#edit-add-wrapper').parent().hide('slow');
  });
  if ($('input#edit-chid-access-settings-inherit').attr('checked')) {
    $('#edit-add-wrapper').parent().hide();
  }
  if ($('input#edit-chid-access-settings-independent').attr('checked')) {
    $('#edit-add-wrapper').parent().show();
  }
  $('input#edit-chid-access-settings-independent').click(function () {
    $('#edit-add-wrapper').parent().show('slow');
  });
  $('input#edit-chid-access-settings-inherit').click(function () {
    $('#edit-add-wrapper').parent().hide('slow');
  });
});