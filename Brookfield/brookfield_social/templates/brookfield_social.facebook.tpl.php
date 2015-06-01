<?php
/**
 * Available variables:
 *   $pagefeed
 */
// drupal_add_js('sites/all/libraries/jcarousel/src/autoscroll.js');

?>
<div id="brookfield-social-wall" class="well social-block">
  <div id="fb-root"></div>
  <div class="jcarousel-wrapper">
    <div class="jcarousel jcarouselfb">
      <ul class="jcarouselfblist jcarousel-skin-default">
      <?php foreach ($pagefeed['data'] as $key => $data): ?>
        <?php if(!empty($data['picture']) && ($data['type'] === 'photo' || $data['type'] === 'link') && $key < 6): ?>
        <li>
          <div class="brookfield-social-post-wrapper">
            <div class="brookfield-social-share-wrapper">
              <div class="brookfield-social-post-photo">
                <?php if (isset($data['picture']) && isset($data['link'])): ?>
                  <a href="<?php print $data['link']; ?>" target="_blank"><img src="<?php print $data['picture']; ?>"></a>
                <?php endif; ?>
              </div>

              <div class="brookfield-social-share">
                <div class="fb-like" data-href="<?php print $data['link']; ?>" data-layout="box_count" data-action="like" data-show-faces="false" data-share="true"></div>
              </div>
            </div>

            <div class="brookfield-social-post-message">
              <?php if (isset($data['message'])): ?>
                <?php print $title = (strlen($data['message']) > 80) ? substr($data['message'], 0, 80) . '...' : $data['message'];?>
              <?php endif; ?>
            </div>

            <div class="brookfield-social-post-date">
              <?php if (isset($data['created_time'])): ?>
                <?php print strtoupper(date('F j Y', strtotime($data['created_time']))); ?>
              <?php endif; ?>
            </div>

            <div class="brookfield-social-post-link">
            <?php if (isset($data['link'])): ?>
              <?php print l(t('View post on Facebook'), $data['link'], array('attributes' => array('target' => '_blank'))); ?>
            <?php endif; ?>
            </div>

          </div>
        </li>
        <?php endif; ?>
      <?php endforeach; ?>
      </ul>
    </div> <!-- jcarousel -->

  </div> <!-- jcarousel-wrapper -->
</div> <!-- brookfield-social-wall -->
