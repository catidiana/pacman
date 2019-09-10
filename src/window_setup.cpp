struct V2
{
   int32_t x, y;
};

struct V3 {
    uint8_t r, g, b;
};


struct Image {
    V3 *pixels;
    uint32_t w, h;
    GLuint texture;
};

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

V3 getpixel(SDL_Surface *surface, int x, int y)
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
draw_image (Image image_des, Image image_src, uint32_t x_center, uint32_t y_center)
{
    uint32_t x_start = x_center - image_src.w/2;
    uint32_t y_start = y_center - image_src.h/2;
    for (uint32_t y = 0; y < image_src.h; y++)
    {
        for (uint32_t x = 0; x < image_src.w; x++)
        {
            if(image_src.pixels[(image_src.h - y -1)*image_src.w + x].r != 255 ||
                    image_src.pixels[(image_src.h - y -1)*image_src.w + x].g != 255 ||
                    image_src.pixels[(image_src.h - y -1)*image_src.w + x].b != 255)
                image_des.pixels[(y+y_start)*image_des.w + x + x_start] = image_src.pixels[(image_src.h - y -1)*image_src.w + x];
        }
    }
}


static void
uniform_fill (Image image, V3 color)
{

    for (uint32_t i = 0; i < image.h*image.w; i++)
    {
        image.pixels[i] = color;
    }
}


