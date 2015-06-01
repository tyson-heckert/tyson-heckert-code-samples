<?php
/**
 * @file
 * Theme YouTube feed.
 *
 * Available Variables:
 *   - $videos
 */
?>
<div id="brookfield-social-wall" class="well social-block">
  <div id="fb-root"></div>
  <div class="jcarousel-wrapper">
    <div class="jcarousel jcarouselyt">
      <ul class="jcarouselytlist jcarousel-skin-default">
        <?php foreach ($videos as $key => $video): ?>
          <?php if($key < 6): ?>
            <li>
              <div class="brookfield-social-post-wrapper">
                <div class="brookfield-social-share-wrapper">
                  <div class="brookfield-social-post-photo">
                      <a href="<?php print $video['link']; ?>" target="_blank"><img src="<?php print $video['thumb']; ?>"></a>
                  </div>

                  <div class="brookfield-social-share">
                    <div class="fb-like" data-href="<?php print $video['link']; ?>" data-layout="box_count" data-action="like" data-show-faces="false" data-share="true"></div>
                  </div>
                </div>

                <div class="brookfield-social-post-message">
                  <?php if (isset($video['title'])): ?>
                   <?php print $title = (strlen($video['title']) > 90) ? substr($video['title'], 0, 90) . '...' : $video['title'];?>
                  <?php endif; ?>
                </div>

                <div class="brookfield-social-post-date">
                  <?php if (isset($video['published'])): ?>
                    <?php print strtoupper(date('F j Y', strtotime($video['published']))); ?>
                  <?php endif; ?>
                </div>

                <div class="brookfield-social-post-link">
                  <?php print l(t('View on YouTube'), $video['link'], array('attributes' => array('target' => '_blank'))); ?>
                </div>

              </div>
            </li>
          <?php endif; ?>
        <?php endforeach; ?>
      </ul>
    </div> <!-- jcarousel -->

  </div> <!-- jcarousel-wrapper -->
</div> <!-- brookfield-social-wall -->
