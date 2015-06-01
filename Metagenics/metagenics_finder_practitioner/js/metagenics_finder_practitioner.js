
(function ($) {

  $( document ).ready(function() {

    //get the default values of the checkboxes checked when we reload the page
    //I did this by sending the function a value that sets what id the item list is
    //all the item links use classes and remain the same, the id makes sure we're only editing the list we want
      checkbox_defaults('credentials');
      checkbox_defaults('specialty');

      //attach our click event to the view all link, it esentially fakes a click on the modal link since recerating another modal link isn't that easy
    $('.item-more-text a').live('click', function(e){
      e.preventDefault();
      $(this).parent().parent().parent().find('.ctools-use-modal').click();
    });

/*
      $('.views-submit-button #edit-field-location-geolocation-latlon-location-geocode').click(function(){
        $('#edit-field-location-geolocation-latlon-wrapper #edit-field-location-geolocation-latlon-location-geocode').click();
      });
*/
      //technically this is a textfield but we don't want people messing with it
     $('#edit-field-location-geolocation-latlon-location-radius').attr("disabled", "disabled");

      //when using the x link to remove a search item, we have a lot of work to do...
      var form_selector = '#views-exposed-form-find-a-practitioner-initial-results-page';

        $(form_selector+' .item-holding-pen .item-link .item-remover').live('click', function(){
          //grab the credential off the unique id we set for this div
          var credential = $(this).parent().attr('id').substring(10).replace(/\-/g, ' ');

          //uncheck the hidden checkbox
          $(form_selector+' input[type=checkbox][value='+credential+']').removeAttr('checked');

          //take a look at all our hidden credentials and show the next one
          //var $(form_selector+' .credential-holding-pen .credential-hidden:first').removeClass('credential-hidden');

          var link = $(this).parent().parent().find('.item-hidden:first');
          if(link.length){
            link.removeClass('item-hidden');
          }

         var links = $(this).parent().parent().find('.item-hidden');

          var link_count = links.length;
          if(link_count <= 0){
              $(this).parent().parent().parent().find('.item-more-text').remove();
          }

          //pop this item off the credential list
          $(this).parent().remove();

         });

    //ajax necessary to pull in practitioner contact info when clicking the contact info link from the view results
      $('.view-display-id-initial_results_page .fap-contact').click(function(e){
        e.preventDefault();

        var href_parts = $(this).attr("href").split('/');
        var fap_id = href_parts[3];

        var url = $(this).attr("href");

        var selector = $(this).parent().parent();

        if ($(this).parent().parent().find('.fap-contact-pop:first').is(":hidden")) {

          //close other contact info windows first
          $('.view-display-id-initial_results_page').find('.fap-contact-pop-wrapper:visible').slideUp();

          $(this).parent().parent().find('.fap-contact-pop-wrapper').slideDown(function(){

             if(!selector.find('.fap-contact-info-wrapper').hasClass('fap-processed')){
               $.ajax({
                  url: url,
                  beforeSend: function ( ) {
                    selector.find('.fap-contact-spinner').show();
                    selector.find('.fap-contact-info-message').hide();
                    $(this).find('.fap-contact-pop').slideDown();
                  },
                  success: function(data, textStatus, jqXHR) {
                    selector.find('.fap-contact-spinner').hide();
                    selector.find('.fap-contact-info-wrapper').addClass('fap-processed').show();

                    var fap_info = data.split(',');
                    if (fap_info[0]) {
                      selector.find('.fap-phone').html(fap_info[0]);
                    }
                    else {
                      selector.find('.fap-phone-wrapper').hide();
                    }

                    if (fap_info[1]) {
                      selector.find('.fap-email').html('<a href="mailto:'+fap_info[1]+'">'+fap_info[1]+'</a>');
                    }
                    else {
                      selector.find('.fap-email-wrapper').hide();
                    }

                  },
                  error: function(jqXHR, textStatus, errorThrown){
                    alert(textStatus);
                  }
                });
            }
            else{
              selector.find('.fap-contact-info-message').hide();
              selector.find('.fap-contact-info-wrapper').show();
            }
          });

        } else {
          $(this).parent().parent().find('.fap-contact-pop-wrapper').slideUp();
        }

        return false;

      });

  //closin dat box
    $('.view-display-id-initial_results_page .fap-contact-pop-close-wrapper').click(function(e){
      $(this).parent().parent().slideUp();
    });

     /*$('.view-display-id-initial_results_page .fap-shop-now').click(function(e){
      e.preventDefault();
      $(this).parent().parent().find('.fap-contact-pop-wrapper').slideDown();
      $(this).parent().parent().find('.fap-contact-info-message').show();
      $(this).parent().parent().find('.fap-contact-spinner').hide();
      $(this).parent().parent().find('.fap-contact-info-wrapper').hide();
     }); */

  });

  function checkbox_defaults(checkbox_type){

    var form_selector = '#views-exposed-form-find-a-practitioner-initial-results-page';

    if(checkbox_type == 'credentials'){
      var checkbox_selector = '#edit-field-practitioner-reference-field-credentials-wrapper';
      var holding_id = '#edit-credentials-holding';
    }
    else{
      var checkbox_selector = '#edit-field-practitioner-reference-field-health-condition-focus-wrapper';
      var holding_id = '#edit-specialty-holding';
    }

        //when the page is refreshed and the checkboxes still checked, add the links, which wont exist now
          $(checkbox_selector+' input:checked').each(function(){
              var item = $(this).val();
              var item = item.replace(/ /g, '-');
              var item = item.replace(/\//g, '');
              var item_text = $("label[for='"+$(this).attr('id')+"']").text();
              var item_link = '<div id="item-link-'+item+'" class="item-link"><div class="item-remover">X</div>'+item_text+'</div>';

              //if we already have credentials in the holding pen, append to the list, otherwise start the list
              var links = $(form_selector+' '+holding_id+' .item-holding-pen .item-link');
              var link_count = links.length;

              if(link_count > 0){
                //if we're over 5 credentials already, add any more as hidden ones until the existing ones are removed
                if(link_count >= 5){
                  //same link but with an extra class to keep it hidden until we need it
                  item_link = '<div id="item-link-'+item+'" class="item-link item-hidden"><div class="item-remover">X</div>'+item_text+'</div>';
                  $(form_selector+' '+holding_id+' .item-holding-pen').append(item_link);

                  //finally, if this is the first time we're going over 5 links, show text to indicate that
                  if($(form_selector+' '+holding_id+' .item-more-text').length == 0){
                    $(form_selector+' '+holding_id+' .item-holding-pen').parent().append('<div class="item-more-text"><a href="#">view all</a></div>');
                  }

                }
                else{
                  $(form_selector+' '+holding_id+' .item-holding-pen').append(item_link);
                }

              }
              else{
                $(form_selector+' '+holding_id+' .item-holding-pen').html(item_link);
              }
          });
  }

}
)(jQuery);

