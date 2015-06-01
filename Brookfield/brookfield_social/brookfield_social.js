Drupal.behaviors.brookfieldFB = {
  attach: function (context, settings) {
    // Facebook Like/Share buttons.
    window.fbAsyncInit = function() {
    FB.init({
      appId      : settings.brookfieldFB.appId,
      status     : true,
      xfbml      : true
    });
    };

    (function(d, s, id){
     var js, fjs = d.getElementsByTagName(s)[0];
     if (d.getElementById(id)) {return;}
     js = d.createElement(s); js.id = id;
     js.src = "//connect.facebook.net/en_US/all.js";
     fjs.parentNode.insertBefore(js, fjs);
    }(document, 'script', 'facebook-jssdk'));
  }
};
/*
(function($) {
    $(function() {
        $('.block-brookfield-social .jcarousel').jcarousel({
          wrap: 'circular'
        });

        $('.block-brookfield-social .jcarouselfb')
          .jcarouselAutoscroll({
            interval: 10000
          });

        $('.block-brookfield-social .jcarouselyt')
          .jcarouselAutoscroll({
            interval: 12000
          });

        $('.block-brookfield-social .jcarousel-control-prev')
            .on('jcarouselcontrol:active', function() {
                $(this).removeClass('inactive');
            })
            .on('jcarouselcontrol:inactive', function() {
                $(this).addClass('inactive');
            })
            .jcarouselControl({
                target: '-=1'
            });

        $('.block-brookfield-social .jcarousel-control-next')
            .on('jcarouselcontrol:active', function() {
                $(this).removeClass('inactive');
            })
            .on('jcarouselcontrol:inactive', function() {
                $(this).addClass('inactive');
            })
            .jcarouselControl({
                target: '+=1'
            });

        $('.block-brookfield-social .jcarousel-pagination')
            .on('jcarouselpagination:active', 'a', function() {
                $(this).addClass('active');
            })
            .on('jcarouselpagination:inactive', 'a', function() {
                $(this).removeClass('active');
            })
            .jcarouselPagination();
    });
})(jQuery);

*/
