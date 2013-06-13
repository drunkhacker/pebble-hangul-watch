#include "pebble_os.h"
#include "pebble_app.h"

typedef struct CharLayer {
  Layer layer;
  const char *c;
  bool bold;
} CharLayer;

#define MY_UUID {0x71, 0x12, 0xD6, 0x24, 0x65, 0x55, 0x42, 0x79, 0x89, 0xAE, 0xC0, 0x86, 0xE9, 0x36, 0x94, 0x27}
PBL_APP_INFO(MY_UUID,
       "Hangul#1", "Drunkhacker.net",
       0, 1, /* App major/minor version */
       DEFAULT_MENU_ICON,
       APP_INFO_WATCH_FACE);

Window window;

ResHandle light_font;
ResHandle bold_font;

void char_layer_update_proc(CharLayer *char_layer, GContext *ctx);

void char_layer_init(CharLayer *char_layer, GRect frame, bool b) {
  layer_init(&char_layer->layer, frame);
  char_layer->layer.update_proc = (LayerUpdateProc)char_layer_update_proc;
  char_layer->bold = b;
}

void char_layer_set_char(CharLayer *char_layer, const char *c) {
  char_layer->c = c;
}

void hangul_char_split(const char *c, int *cho, int *jung, int *jong) {
  int d = ((c[0] & 0x0F) << 12) | ((c[1] & 0x3F) << 6) | (c[2] & 0x3F);
  d -= 44032;
  *cho = d / (21*28);
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

static int chomap[36][3];
static int jungmap[36][3];
static int jongmap[36][3];

uint8_t cachedBitmapData[3456];
int cachePointer;

uint8_t*getNewBitmapBuffer(int size) {
  uint8_t *p = cachedBitmapData + cachePointer;
  cachePointer += size;
  return p;
}

void get_bitmap_cached(int map[][3], int addr, uint8_t **data, bool bold) {
  int i;
  for (i=0;i<36;i++) {
    if (map[i][0] == addr) { //hit
      break;
    }
  }
  if (i == 36) { //nohit
    for (i=0;i<36;i++) { if (map[i][0] == 0) break; }
    map[i][0] = addr;
    map[i][1] = (int)getNewBitmapBuffer(32);
    map[i][2] = (int)getNewBitmapBuffer(32);

    //load resource
    resource_load_byte_range(light_font, addr, (uint8_t*)map[i][1], 32);
    resource_load_byte_range(bold_font, addr, (uint8_t*)map[i][2], 32);
  }

  *data = bold ? (uint8_t*)map[i][2] : (uint8_t*)map[i][1];
}

void char_layer_update_proc(CharLayer *char_layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, char_layer->layer.bounds, 0, GCornerNone);
  //parse char
  int c, j, j_;
  hangul_char_split(char_layer->c, &c, &j, &j_);

  int cb_i = cho_bul[(j_ == 0 ? 0 : 1)*21 + j];
  int cho_offset = (cb_i*20 + (c+1))*32;

  int jb_i = jung_bul[(j_ == 0 ? 0 : 1)*19 + c];
  int jung_offset = (160 + jb_i*22 + (j+1))*32;

  int jjb_i = jong_bul[j];
  int jong_offset = (248 + jjb_i*28 + j_)*32;

  int i;

  //get binary data
  uint8_t *cho_bitmap;
  uint8_t *jung_bitmap;
  uint8_t *jong_bitmap;

  get_bitmap_cached(chomap, cho_offset, &cho_bitmap, char_layer->bold);
  get_bitmap_cached(jungmap, jung_offset, &jung_bitmap, char_layer->bold);
  get_bitmap_cached(jongmap, jong_offset, &jong_bitmap, char_layer->bold);


  //draw actual bitmap
  int x, y;
  GPoint p;
  GRect f = layer_get_frame(&char_layer->layer);
  for (y=0;y<16;y++) {
    p.y = y + f.origin.y;
    for (j=0;j<2;j++) {
      uint8_t line1 = *cho_bitmap;
      uint8_t line2 = *jung_bitmap;
      uint8_t line3 = *jong_bitmap;
      uint8_t line = line1 | line2 | line3;

      for (x=0;x<8;x++) {
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
    return;
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


CharLayer layers[36];

// Called once per second
PblTm currentTime;
void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  //get old indexes
  int old_indexes[11] = {-1,};
  get_char_indexes(currentTime.tm_hour, currentTime.tm_min, old_indexes);

  get_time(&currentTime);

  int new_indexes[11] = {-1,};
  get_char_indexes(currentTime.tm_hour, currentTime.tm_min, new_indexes);

  int *pc = old_indexes;
  while(*pc >= 0) { 
    layers[*pc].bold = false;
    layer_mark_dirty(&layers[*pc].layer);
    pc++;
  }
  pc = new_indexes;
  while(*pc >= 0) { 
    layers[*pc].bold = true;
    layer_mark_dirty(&layers[*pc].layer);
    pc++;
  }

}

void handle_deinit(AppContextRef ctx) {
}
void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Hangul Watch");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&HANGUL_WATCH_RESOURCES);

  //load resource
  light_font = resource_get_handle(RESOURCE_ID_FONT_HAN_LIGHT);
  bold_font = resource_get_handle(RESOURCE_ID_FONT_HAN_BOLD);

  //cache map init
  memset(chomap, 0, 108);
  memset(jungmap, 0, 108);
  memset(jongmap, 0, 108);

  cachePointer = 0;

  //layer initialization (36개)

  int r, c;
  for (r=0;r<6;r++) {
    for (c=0;c<6;c++) {
      CharLayer *pLayer = &layers[r*6+c];
      char_layer_init(pLayer, GRect(xOffset + c*24, r*24 + yOffset ,24,24), false);
      char_layer_set_char(pLayer, characters[r*6+c]);
      layer_add_child(&window.layer, &pLayer->layer);
    }
  }

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  get_time(&currentTime);
  handle_second_tick(ctx, NULL);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = MINUTE_UNIT
    }
  };

  app_event_loop(params, &handlers);
}

