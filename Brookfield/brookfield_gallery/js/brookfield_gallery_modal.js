/**
 * Limit allowed number of images to share.
 */
(function ($) {
  Drupal.behaviors.brookfieldGallery = {
    attach: function (context, settings) {
      $(".brookfield-gallery-email-photo-form input[type=checkbox]").click(function() {
        var bol = $("input[type=checkbox]:checked").length >= 6;
        $("input[type=checkbox]").not(":checked").attr("disabled",bol);
      });

    $('.use-ajax', context).click(function(){
        $('.gallery-loading-image').css('z-index', 1);

      });
      $('#master-wrapper').bind("DOMSubtreeModified",function(){
          $('.gallery-loading-image').css('z-index', -1);
      });

      // Change the text of the edit button on the bulk upload form
      $('.view-gallery-admin button.editablefield-edit').html('<i class="fa fa-pencil-square-o"></i>').addClass('btn-xs');
    }
  }
})(jQuery);

/**
 * Provide the HTML to create the modal dialog in the Bootstrap style.
 */
Drupal.theme.prototype.BrookfieldGalleryModalDialog = function () {
  var html = ''
  html += '  <div id="ctools-modal gallery-upload-modal">'
  html += '    <div class="ctools-modal-dialog gallery-modal">'
  html += '      <div class="modal-content">'
  html += '        <div class="modal-header">';
  html += '          <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>';
  html += '          <h4 id="modal-title" class="modal-title">&nbsp;</h4>';
  html += '        </div>';
  html += '        <div id="modal-content" class="modal-body">';
  html += '        </div>';
  html += '      </div>';
  html += '    </div>';
  html += '  </div>';

  return html;
}

/**
 * Provide the HTML to create a nice looking loading progress bar.
 */
Drupal.theme.prototype.BrookfieldGalleryModalThrobber = function () {
  var html = '';
  html += '  <div class="loading-spinner" style="width: 200px; margin: -20px 0 0 -100px; position: absolute; top: 45%; left: 50%">';
  html += '    <div class="progress progress-striped active">';
  html += '      <div class="progress-bar" style="width: 100%;"></div>';
  html += '    </div>';
  html += '  </div>';

  return html;
};
