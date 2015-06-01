(function ($) {
  Drupal.behaviors.metaRecurringOrderForm = {
    attach: function (context, settings) {
      var lineItemNewInput = $('input.line-items-new-product-id');

      lineItemNewInput.focus(function() {
        if (lineItemNewInput.val() == lineItemNewInput.attr('placeholder')) {
          $(this).val('');
        }
      });

      lineItemNewInput.blur(function() {
        if(lineItemNewInput.val() == '') {
          $(this).val(lineItemNewInput.attr('placeholder'));
        }
      });

      $('.open-next-date').click(function(){
        $('.field-name-field-aro-next-ship-date').slideDown();
        $(this).fadeOut();
        return false;
      });

      $('.close-next-date').click(function(){
        $('.field-name-field-aro-next-ship-date').slideUp();
        $('.open-next-date').fadeIn();
        return false;
      });

      $('.field-name-field-aro-next-ship-date input.form-text').change(function(){
        console.log(this.value);
        $('.next-ship-date-display').text(this.value).addClass('date-changed');

      });

      // Datepicker limit days options.
      for (var id in Drupal.settings.datePopup) {
        //Drupal.settings.datePopup[id].settings.beforeShowDay = customDates;
        Drupal.settings.datePopup[id].settings.minDate = '+1d';
      }
    }
  }
})(jQuery);

function customDates(date) {
  return [(date.getDay() != 0), ''];
}

