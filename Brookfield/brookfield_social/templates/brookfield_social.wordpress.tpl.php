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
        <?php foreach ($videos['entry'] as $key => $data): ?>
          <?php if(!empty($data['content']) && $key < 6): ?>
            <li>
              <div class="brookfield-social-post-wrapper">
                <div class="brookfield-social-share-wrapper">
                  <div class="brookfield-social-post-photo">
                    <?php if (isset($data['content']) && isset($data['id'])): ?>
                      <?php $url = explode('/', $data['id']); $id = array_pop($url); ?>
                      <a href="<?php print $data['link'][0]['@attributes']['href']; ?>" target="_blank"><img src="http://img.youtube.com/vi/<?php print $id; ?>/hqdefault.jpg"></a>
                    <?php endif; ?>
                  </div>

                  <div class="brookfield-social-share">
                    <div class="fb-like" data-href="<?php print $data['link'][0]['@attributes']['href']; ?>" data-layout="box_count" data-action="like" data-show-faces="false" data-share="true"></div>
                  </div>
                </div>

                <div class="brookfield-social-post-message">
                  <?php if (isset($data['title'])): ?>
                   <?php print $title = (strlen($data['title']) > 90) ? substr($data['title'], 0, 90) . '...' : $data['title'];?>
                  <?php endif; ?>
                </div>

                <div class="brookfield-social-post-date">
                  <?php if (isset($data['published'])): ?>
                    <?php print strtoupper(date('F j Y', strtotime($data['published']))); ?>
                  <?php endif; ?>
                </div>

                <div class="brookfield-social-post-link">
                <?php if (isset($data['id'])): ?>
                  <?php print l(t('View on YouTube'), $data['link'][0]['@attributes']['href'], array('attributes' => array('target' => '_blank'))); ?>
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
