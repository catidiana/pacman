/* PacMan Game
 *
 * Copyright (C) 2019 Martin & Diana
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

struct V2
{
   uint32_t x, y;
};

struct V3 {
    uint8_t r, g, b;
};


struct Image {
    V3 *pixels;
    uint32_t w, h;
    GLuint texture;
};

typedef Mix_Chunk * Sound;

static void
set_window_transform (int window_w, int window_h)
{
    float w = 2.0f / window_w;
    float h = 2.0f / window_h;
    float x = 0;
    float y = 0;

    float transform[] = {
        w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, 1, 0,
        x, y, 0, 1,
    };

    glLoadMatrixf (transform);
    glViewport (0, 0, window_w, window_h);
}


static Image
new_image (uint32_t w, uint32_t h)
{
    Image image = {};
    image.w = w;
    image.h = h;
    image.pixels = (V3 *) malloc (sizeof (V3) * image.w * image.h);

    glGenTextures (1, &image.texture);
    glBindTexture (GL_TEXTURE_2D, image.texture);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return image;
}


static void
update_image_texture (Image image)
{
    glBindTexture (GL_TEXTURE_2D, image.texture);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB8,
                  image.w, image.h, 0,
                  GL_RGB, GL_UNSIGNED_BYTE,
                  image.pixels);
}


static void
show_image (Image image)
{
    int w = image.w / 2;
    int h = image.h / 2;

    int x0 =  - w;
    int x1 =  + w;
    int y0 =  - h;
    int y1 =    h;

    glBindTexture (GL_TEXTURE_2D, image.texture);

    glBegin (GL_TRIANGLE_STRIP);

    glTexCoord2f (0, 0);
    glVertex2f (x0, y0);
    glTexCoord2f (1, 0);
    glVertex2f (x1, y0);
    glTexCoord2f (0, 1);
    glVertex2f (x0, y1);
    glTexCoord2f (1, 1);
    glVertex2f (x1, y1);

    glEnd ();
}

static V3
getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    uint8_t *p = (uint8_t *)surface->pixels + y * surface->pitch + x * bpp;
    return {p[0], p[1], p[2]};
}

static Image
load_image (const char *filename)
{
    SDL_Surface *surface;

    surface = IMG_Load(filename);
    if (!surface) {
        printf("Picture is not found: %s\n", filename);
        assert(false);
    }

    int textw=surface->w;
    int texth=surface->h;
    Image image_src = new_image (textw, texth);
    for (uint32_t y = 0; y < image_src.h; y++)
    {
        for (uint32_t x = 0; x < image_src.w; x++)
        {
            image_src.pixels[image_src.w*y + x] = getpixel(surface, x, y);
        }
    }
    SDL_FreeSurface(surface);
    return image_src;
}


static void
draw_image (Image des, Image src, u32 x_center, u32 y_center, u32 w, u32 h, u32 x_offset, u32 y_offset)
{
    u32 x_start = x_center - w / 2;
    u32 y_start = y_center - h / 2;
    u32 src_y = h - 1 + y_offset;
    u32 des_y = y_start;

    for (u32 y = 0; y < h; y++)
    {
        u32 src_x = x_offset;
        u32 des_x = x_start;
        for (u32 x = 0; x < w; x++)
        {
            V3 pix = src.pixels[src_y * src.w + src_x];
            if (pix.r != 255 || pix.g != 255 || pix.b != 255)
            {
               des.pixels[des_y * des.w + des_x] = pix;
            }
            ++src_x;
            ++des_x;
        }
        --src_y;
        ++des_y;
    }
}


static void
draw_image (Image des, Image src, u32 x_center, u32 y_center)
{
  draw_image (des, src, x_center, y_center, src.w, src.h, 0, 0);
}


static void
uniform_fill (Image image, V3 color)
{

    for (uint32_t i = 0; i < image.h*image.w; i++)
    {
        image.pixels[i] = color;
    }
}

static Sound
load_sound (const char *filename)
{

  Mix_Chunk *sound = Mix_LoadWAV (filename);
  if (!sound)
    {
      printf ("File not found %s\n", filename);
      assert (sound);
      return 0;
    }

  return sound;
}


static void
play_sound (Mix_Chunk *sound)
{
  Mix_PlayChannel (-1, sound, 0);
}
