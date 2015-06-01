<?php
/**
 * @file
 * Theme YouTube feed.
 *
 * Available Variables:
 *   - $deals
 */
?>
<div id="brookfield-social-wall" class="well social-block dealcatcher-wrapper">
  <div class="jcarousel-wrapper">
    <div class="jcarousel jcarouseldc jcarousel-skin-default">
      <ul class="">

        <?php foreach ($deals->deals as $key => $data): ?>
          <?php if(!empty($data)): ?>
            <li>
              <div class="brookfield-social-post-wrapper">

                <div class="brookfield-social-post-photo">
                  <?php if (!empty($data->largeImageUrl) && !empty($data->dealUrl)): ?>
                    <a href="<?php print $data->dealUrl; ?>" target="_blank"><img src="<?php print $data->largeImageUrl; ?>"></a>
                  <?php endif; ?>
                </div>
                <div class="dealcatcher-details">
                <div class="brookfield-social-post-merchant">
                  <?php if (!empty($data->merchant->name)): ?>
                    <a href="<?php print $data->dealUrl; ?>" target="_blank"><?php print $data->merchant->name; ?></a>
                  <?php endif; ?>
                </div>

                <div class="brookfield-social-post-title">
                  <?php if (!empty($data->announcementTitle)): ?>
                   <?php print $data->announcementTitle; ?>
                  <?php endif; ?>
                </div>

                <div class="brookfield-social-post-location">
                  <?php if (!empty($data->division->name)): ?>
                    <span class="icon fa fa-map-marker"><span><?php print strtoupper($data->division->name); ?></span></span>
                  <?php endif; ?>
                </div>

                <div class="brookfield-social-post-price-wrap">
                <div class="brookfield-social-post-value">
                <?php if (!empty($data->options[0]->value)): ?>
                  <?php print $data->options[0]->value->formattedAmount; ?>
                <?php endif; ?>
               </div>
                <div class="brookfield-social-post-price">
                <?php if (!empty($data->options[0]->price)): ?>
                  <?php print $data->options[0]->price->formattedAmount; ?>
                <?php endif; ?>
                </div>

                </div>


              </div>
              </div>
            </li>
          <?php endif; ?>
        <?php endforeach; ?>
      </ul>
    </div> <!-- jcarousel -->



  </div> <!-- jcarousel-wrapper -->
</div> <!-- brookfield-social-wall -->
