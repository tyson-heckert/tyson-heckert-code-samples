
(function ($) {

   //wrap this in drupal behaviors so we can include settings variable letting us know what type of credential list this is
   Drupal.behaviors.fapCredentialChecker = {
      attach: function(context, settings) {

	  //threw this in here to cut down on character count, it's a long id
      var form_selector = '#views-exposed-form-find-a-practitioner-initial-results-page';

      //pull in the settings so we know what credential list to add to
      var checkbox_type = Drupal.settings.metagenics_finder_practitioner.var_form_type;

      //set our selectors here so we aren't copy pasting code everywhere
      if(checkbox_type == 'credentials'){
        var checkbox_selector = '#edit-field-practitioner-reference-field-credentials-wrapper';
        var holding_id = '#edit-credentials-holding #edit-pen';
        var old_checkboxes = [];
        $(checkbox_selector+' input[type=checkbox]:checked').each(function(){
          old_checkboxes.push($(this).val());
        });
        var old_pen = $(holding_id).html();
      }
      else{
        var checkbox_selector = '#edit-field-practitioner-reference-field-health-condition-focus-wrapper';
        var holding_id = '#edit-specialty-holding #edit-pen--2';
        var old_checkboxes = [];
        $(checkbox_selector+' input[type=checkbox]:checked').each(function(){
          old_checkboxes.push($(this).val());
        });
        var old_pen = $(holding_id).html();
      }

      //close listeners
      $('.fap-item-close-nosave').click(function(){
        $(checkbox_selector+' input[type=checkbox]:checked').removeAttr('checked');
          $.each(old_checkboxes, function(){
            $(checkbox_selector+' input[type=checkbox][value='+this+']').attr('checked', 'checked');
          });
         $(holding_id).html(old_pen);
         Drupal.CTools.Modal.dismiss();
      });
      $('.fap-item-close-save').click(function(){
        Drupal.CTools.Modal.dismiss();
      });

    	//when the ctools modal is loaded, all the checked credentials should be pre-selected
    	  $(checkbox_selector+' input:checked').each(function(){

    	  	  var item = $(this).val();

    	  	$('#metagenics-finder-practitioner-criteria-form input[type=checkbox][value='+item+']').attr('checked', 'checked');
    	  });

    	  //when checking an item in the modal, check it on the main screen's hidden checkbox as well
          $('#metagenics-finder-practitioner-criteria-form .form-checkbox').click(function(){

            //set up our variables
          if(checkbox_type == 'credentials'){
            var item = $(this).val();
            var item_val = $(this).val();
          }
          else{
            var item = $(this).val().replace(/ /g, '-');
            var item = item.replace(/\//g, '');
            var item_val = $(this).val();
          }

            var item_text = $("label[for='"+$(this).attr('id')+"']").text();
            var item_link = '<div id="item-link-'+item+'" class="item-link"><div class="item-remover">X</div>'+item_text+'</div>';
            var links = $(form_selector+' '+holding_id+' .item-holding-pen .item-link');
            var link_count = links.length;

            //we're checking it and have to add stuff to the main search page
            if($(this).is(':checked')){
              //check the hidden checkbox on the main search screen
              $(checkbox_selector+' input[type=checkbox][value='+item_val+']').attr('checked', 'checked');

              //if we already have items in the holding pen, append to the list, otherwise start the list
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
            }

            //we're unchecking it and have to remove stuff from the main screen
            else{

              //uncheck the actual checkbox
              $(checkbox_selector+' input[type=checkbox][value='+item_val+']').removeAttr('checked');

                //if we're over 5 items already we have to look and see if we're going to show hidden ones
                if(link_count >= 5){
                    //grab the specific link from the main search screen
                    var link_obj = $(form_selector+' '+holding_id+' .item-holding-pen'+' #item-link-'+item);

                    //if it's a hidden link, we remove it and see if the ...more text should stay or go
                    if(link_obj.hasClass('item-hidden')){

                      var links = link_obj.parent().find('.item-hidden');

                      var link_count = links.length;
                      if(link_count <= 0){
                          link_obj.parent().parent().find('.item-more-text').remove();
                      }

                      link_obj.remove();
                    }

                    //if its a shown link, we remove it, show the next in the list of hidden items, then decide if the more text should stay or go
                    else{

                      var link = link_obj.parent().find('.item-hidden:first');

                      if(link.length){
                          link.removeClass('item-hidden');
                      }

                      var links = link_obj.parent().find('.item-hidden');

                      var link_count = links.length;
                      if(link_count <= 0){
                          link_obj.parent().parent().find('.item-more-text').remove();
                      }

                      link_obj.remove();

                    }
                 }
                 //if we're not over five links already, we can just remove what's there
                else{
                  $(form_selector+' '+holding_id+' .item-holding-pen'+' #item-link-'+item).remove();
                }

            }
          });


  $("#modalContent .checkall").bind('click', function () {
      if($(this).is(':checked')){
         $('#modalContent .form-checkbox').each(function(){
          if(!$(this).is(':checked')){
            $(this).attr('checked', 'checked');
            $(this).trigger("click");
            $(this).attr('checked', 'checked');
          }
         });
         $("#modalContent .checknone").removeAttr('checked');
       }
   });
    $("#modalContent .checknone").bind('click', function () {
      if($(this).is(':checked')){
         $('#modalContent .form-checkbox').each(function(){
          if($(this).is(':checked')){
            $(this).removeAttr('checked');
            $(this).trigger("click");
            $(this).removeAttr('checked');
          }
         });
         $("#modalContent .checkall").removeAttr('checked');
       }
   });

/*
    $('.item-more-text a').live('click', function(e){
      e.preventDefault();
      $(this).parent().parent().parent().find('.ctools-use-modal').click();
    });
*/

    }
}
Drupal.attachBehaviors($(this));

}
)(jQuery);

