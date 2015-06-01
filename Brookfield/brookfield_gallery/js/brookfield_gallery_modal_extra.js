(function ($) {
  Drupal.behaviors.brookfieldGalleryExtra = {
    attach: function (context, settings) {

      $('#brookfield-gallery-bulk-media-upload-upload-form .form-submit').attr('disabled','disabled');

      // This is for the add form to show the submit button on upload.
      $("#brookfield-gallery-bulk-media-upload-upload-form .plupload_start").on('click', function(){
        var phase = document.getElementById('edit-default-values-field-gallery-phase-ref-und');
        if( phase.selectedIndex != 0){
          $('#brookfield-gallery-bulk-media-upload-upload-form .form-submit').removeAttr('disabled');
        }
      });

      $("#edit-default-values-field-gallery-phase-ref-und").on('change', function(){
        if($('.plupload_upload_status').is(':visible')){
          $('#brookfield-gallery-bulk-media-upload-upload-form .form-submit').removeAttr('disabled');
        }

      });

    }
  }
})(jQuery);
