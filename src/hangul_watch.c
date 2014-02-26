/**
 *  Original Source from: https://github.com/drunkhacker/pebble-hangul-watch
 *  Original Creator: DrunkHacker, http://blog.drunkhacker.me/?p=292
 *
 *  Modified Date: 2014-2-23, By: A.J <andrwj@gmail.com>
 *  Modified Repository: https://github.com/andrwj/pebble-hangul-watch
 */
#include <pebble.h>
#define  LAYERS  36

typedef struct CharLayer {
  Layer       *layer;
  const char  *c;
  bool        bold;
} CharLayer;

static CharLayer  *layers[LAYERS];

static Window     *window;
static ResHandle  light_font;
static ResHandle  bold_font;
static struct tm  prev_t;

void hangul_char_split(const char *c, int *cho, int *jung, int *jong) {
  int d = ((c[0] & 0x0F) << 12) | ((c[1] & 0x3F) << 6) | (c[2] & 0x3F);
     d -= 44032;
  *cho  = d / (21*28);
  *jung = (d % (21*28)) / 28;
  *jong = d % 28;
}

static int cho_bul[] =  {0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 3, 1, 2, 4, 4, 4, 2, 1, 3, 0,
                         5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 7, 7, 6, 6, 7, 7, 7, 6, 6, 7, 5};
static int jung_bul[] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
                         2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3};
static int jong_bul[] = {0, 2, 0, 2, 1, 2, 1, 2, 3, 0, 2, 1, 3, 3, 1, 2, 1, 3, 3, 1, 1};

static int yOffset = 10;
static int xOffset = 3;

static int chomap[LAYERS][3];
static int jungmap[LAYERS][3];
static int jongmap[LAYERS][3];

uint8_t cachedBitmapData[3456];
int cachePointer;

uint8_t*getNewBitmapBuffer(int size) {
  uint8_t *p = cachedBitmapData + cachePointer;
  cachePointer += size;
  return p;
}

void get_bitmap_cached(int map[][3], int addr, uint8_t **data, bool bold) {
  int i;
  for (i=0;i<LAYERS;i++) {
    if (map[i][0] == addr) {
      break;
    }
  }
  if (i == LAYERS) {
    for (i=0;i<LAYERS;i++) { if (map[i][0] == 0) break; }
    map[i][0] = addr;
    map[i][1] = (int)getNewBitmapBuffer(32);
    map[i][2] = (int)getNewBitmapBuffer(32);

    resource_load_byte_range(light_font, addr, (uint8_t*)map[i][1], 32);
    resource_load_byte_range(bold_font, addr, (uint8_t*)map[i][2], 32);
  }

  *data = bold ? (uint8_t*)map[i][2] : (uint8_t*)map[i][1];
}

static char *characters[] = {
  "오", "전", "후", "열", "한", "두",
  "세", "일", "곱", "다", "여", "섯",
  "네", "여", "덟", "아", "홉", "시",
  "자", "이", "삼", "사", "오", "십",
  "정", "오", "일", "이", "삼", "사",
  "육", "칠", "팔", "구", "분", "초"};

void get_char_indexes(int h, int m, int *indices) {
  int *pc = indices;
  bool pm = h >= 12;

  if (h == 0) {
    if (pm) {
      *pc++ = 24;
      *pc++ = 25;
    } else {
      *pc++ = 18;
      *pc++ = 24;
    }
  }

  if (pm) {
    h -= 12;
    *pc++ = 2;
  } else {
    *pc++ = 1;
  }

  *pc++ = 0;
  *pc++ = 17;

  switch (h) {
    case 0:
      *pc++ = 3;
      *pc++ = 5;
      break;
    case 1:
      *pc++ = 4;
      break;
    case 2:
      *pc++ = 5;
      break;
    case 3:
      *pc++ = 6;
      break;
    case 4:
      *pc++ = 12;
      break;
    case 5:
      *pc++ = 9;
      *pc++ = 11;
      break;
    case 6:
      *pc++ = 10;
      *pc++ = 11;
      break;
    case 7:
      *pc++ = 7;
      *pc++ = 8;
      break;
    case 8:
      *pc++ = 13;
      *pc++ = 14;
      break;
    case 9:
      *pc++ = 15;
      *pc++ = 16;
      break;
    case 10:
      *pc++ = 3;
      break;
    case 11:
      *pc++ = 3;
      *pc++ = 4;
      break;
  }

  if (m >= 10) {
    *pc++ = 23;
    if (m >= 20) {
      *pc++ = m/10 + 17;
    }
  }
  if (m % 10 == 5)
    *pc++ = 25;
  else if (m % 10 <= 4)
    *pc++ = (m % 10) + 25;
  else
    *pc++ = (m % 10) + 24;

  if (m != 0)
    *pc++ = 34;

  *pc = -1;
}

void char_layer_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  CharLayer *pl = (CharLayer *)layer_get_data(layer);

  int c, j, j_;
  hangul_char_split(pl->c, &c, &j, &j_);

  int cb_i = cho_bul[(j_ == 0 ? 0 : 1)*21 + j];
  int cho_offset = (cb_i*20 + (c+1))*32;

  int jb_i = jung_bul[(j_ == 0 ? 0 : 1)*19 + c];
  int jung_offset = (160 + jb_i*22 + (j+1))*32;

  int jjb_i = jong_bul[j];
  int jong_offset = (248 + jjb_i*28 + j_)*32;

  uint8_t *cho_bitmap;
  uint8_t *jung_bitmap;
  uint8_t *jong_bitmap;

  get_bitmap_cached(chomap, cho_offset, &cho_bitmap, pl->bold);
  get_bitmap_cached(jungmap, jung_offset, &jung_bitmap, pl->bold);
  get_bitmap_cached(jongmap, jong_offset, &jong_bitmap, pl->bold);


  int x, y;
  GPoint p;
  GRect f = layer_get_bounds(layer);

  for (y=0;y<16;y++) {
    p.y = y + f.origin.y;
    for (j=0;j<2;j++) {
      uint8_t line1 = *cho_bitmap;
      uint8_t line2 = *jung_bitmap;
      uint8_t line3 = *jong_bitmap;
      uint8_t line = line1 | line2 | line3;
      for (x=0;x<8;x++) {
        if(!pl->bold && (x % 3)) continue;
        p.x = j*8 + x + f.origin.x;
        graphics_context_set_stroke_color(ctx, (0x1 & (line >> (7 - x))) ? GColorWhite : GColorBlack);
        graphics_draw_pixel(ctx, p);
      }
      cho_bitmap++;
      jung_bitmap++;
      jong_bitmap++;
    }
  }
}

static void handle_second_tick(struct tm *t, TimeUnits units_changed) {

  int old_indexes[11] = {-1,};
  get_char_indexes(prev_t.tm_hour, prev_t.tm_min, old_indexes);

  int new_indexes[11] = {-1,};
  get_char_indexes(t->tm_hour, t->tm_min, new_indexes);

  int *pc = old_indexes;
  while(*pc >= 0) {
    layers[*pc]->bold = false;
    layer_mark_dirty(layers[*pc]->layer);
    pc++;
  }
  pc = new_indexes;
  while(*pc >= 0) {
    layers[*pc]->bold = true;
    layer_mark_dirty(layers[*pc]->layer);
    pc++;
  }
  prev_t = *t;
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  light_font = resource_get_handle(RESOURCE_ID_FONT_HAN_LIGHT);
  bold_font = resource_get_handle(RESOURCE_ID_FONT_HAN_BOLD);

  memset(chomap,  0, sizeof(chomap));
  memset(jungmap, 0, sizeof(jungmap));
  memset(jongmap, 0, sizeof(jongmap));

  cachePointer = 0;

  int r, c;
  Layer     *layer;
  CharLayer *pl;

  for (r=0;r<6;r++) {
    for (c=0;c<6;c++) {
      layer           = layer_create_with_data(GRect(xOffset + c*24, r*24 + yOffset ,24,24), sizeof(CharLayer));
      pl              = layer_get_data(layer);
      pl->layer       = layer;
      pl->bold        = false;
      pl->c           = characters[r*6+c];
      layers[r*6+c]   = pl;
      layer_add_child(window_layer, layer);
      layer_set_update_proc(layer, char_layer_update_proc);
    }
  }

  time_t now = time(0);
  prev_t = *localtime(&now);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);
}

static void window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  for(int cnt=0; cnt<LAYERS; cnt++) {
      layer_destroy(layers[cnt]->layer);
  }
}

static void init(void) {
  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}
