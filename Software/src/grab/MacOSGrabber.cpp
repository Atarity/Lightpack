#include"MacOSGrabber.hpp"

#ifdef MAC_OS_GRAB_SUPPORT
#include<CoreFoundation/CoreFoundation.h>
#include<ApplicationServices/ApplicationServices.h>
#include"qmacdefines_mac.h"
#include"debug.h"
#include"OpenGL/OpenGL.h"
#include"OpenGL/gl.h"

MacOSGrabber::MacOSGrabber()
{
}

MacOSGrabber::~MacOSGrabber()
{
}

const char * MacOSGrabber::getName()
{
    return "MacOSGrabber";
}

void MacOSGrabber::updateGrabScreenFromWidget(QWidget *widget)
{
}

QList<QRgb> MacOSGrabber::grabWidgetsColors(QList<GrabWidget *> &widgets)
{
    CGDisplayCount displayCount;
    CGDirectDisplayID displays[32];

    // grab the active displays
    CGGetActiveDisplayList(32, displays, &displayCount);
    CGRect bounds = CGDisplayBounds(displays[0]);
    CGImageRef image = openGlGrab();//CGDisplayCreateImageForRect(displays[0], bounds);
    QPixmap pixmap = QPixmap::fromMacCGImageRef(image);
    QList<QRgb> result;
    for(int i = 0; i < widgets.size(); i++) {
        result.append(getColor(pixmap, widgets[i]));
    }
    CGImageRelease(image);
    return result;
}

CGImageRef MacOSGrabber::openGlGrab()
{
    CGDirectDisplayID display = kCGDirectMainDisplay;
    CGRect srcRect = {{0, 0}, {1024, 768}};
    CGContextRef bitmap;
    CGImageRef image;
    void * data;
    long bytewidth;
    GLint width, height;
    long bytes;
    CGColorSpaceRef cSpace = CGColorSpaceCreateWithName (kCGColorSpaceGenericRGB);

    CGLContextObj    glContextObj;
    CGLPixelFormatObj pixelFormatObj ;
    long numPixelFormats ;
    CGLPixelFormatAttribute attribs[] =
    {
        kCGLPFAFullScreen,
        kCGLPFADisplayMask,
        (CGLPixelFormatAttribute)CGDisplayIDToOpenGLDisplayMask(display),
        (CGLPixelFormatAttribute)0
    } ;


    if ( display == kCGNullDirectDisplay )
        display = CGMainDisplayID();


    /* Build a full-screen GL context */
    CGLError err = CGLChoosePixelFormat( attribs, &pixelFormatObj, (GLint *)&numPixelFormats );
    DEBUG_OUT << err;
    if ( pixelFormatObj == NULL )    // No full screen context support
        return NULL;
    CGLCreateContext( pixelFormatObj, NULL, &glContextObj ) ;
    CGLDestroyPixelFormat( pixelFormatObj ) ;
    if ( glContextObj == NULL )
        return NULL;


    CGLSetCurrentContext( glContextObj ) ;
    CGLSetFullScreen( glContextObj ) ;


    glReadBuffer(GL_FRONT);


    width = srcRect.size.width;
    height = srcRect.size.height;


    bytewidth = width * 4; // Assume 4 bytes/pixel for now
    bytewidth = (bytewidth + 3) & ~3; // Align to 4 bytes
    bytes = bytewidth * height; // width * height

    /* Build bitmap context */
    data = malloc(height * bytewidth);
    if ( data == NULL )
    {
        CGLSetCurrentContext( NULL );
        CGLClearDrawable( glContextObj ); // disassociate from full screen
        CGLDestroyContext( glContextObj ); // and destroy the context
        return NULL;
    }
    bitmap = CGBitmapContextCreate(data, width, height, 8, bytewidth,
                                   cSpace, kCGImageAlphaNoneSkipFirst /* XRGB */);
    CFRelease(cSpace);


    /* Read framebuffer into our bitmap */
    glFinish(); /* Finish all OpenGL commands */
    glPixelStorei(GL_PACK_ALIGNMENT, 4); /* Force 4-byte alignment */
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

    /*
     * Fetch the data in XRGB format, matching the bitmap context.
     */
    glReadPixels((GLint)srcRect.origin.x, (GLint)srcRect.origin.y, width, height,
                 GL_BGRA,
#ifdef __BIG_ENDIAN__
                 GL_UNSIGNED_INT_8_8_8_8_REV, // for PPC
#else
                 GL_UNSIGNED_INT_8_8_8_8, // for Intel! http://lists.apple.com/archives/quartz-dev/2006/May/msg00100.html
#endif
                 data);
    /*
     * glReadPixels generates a quadrant I raster, with origin in the lower left
     * This isn't a problem for signal processing routines such as compressors,
     * as they can simply use a negative 'advance' to move between scanlines.
     * CGImageRef and CGBitmapContext assume a quadrant III raster, though, so we need to
     * invert it. Pixel reformatting can also be done here.
     */
//    swizzleBitmap(data, bytewidth, height);


    /* Make an image out of our bitmap; does a cheap vm_copy of the bitmap */
    image = CGBitmapContextCreateImage(bitmap);

    /* Get rid of bitmap */
    CFRelease(bitmap);
    free(data);


    /* Get rid of GL context */
    CGLSetCurrentContext( NULL );
    CGLClearDrawable( glContextObj ); // disassociate from full screen
    CGLDestroyContext( glContextObj ); // and destroy the context

    /* Returned image has a reference count of 1 */
    return image;
}
QRgb MacOSGrabber::getColor(QPixmap pixmap, const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return getColor(pixmap,
                    grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb MacOSGrabber::getColor(QPixmap pixmap, int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
                     << "x y w h:" << x << y << width << height;

    QPixmap pix = pixmap.copy(x,y, width, height);
    QPixmap scaledPix = pix.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage im = scaledPix.toImage();
    QRgb result = im.pixel(0,0);

    DEBUG_HIGH_LEVEL << "QRgb result =" << hex << result;

    return result;
}
#endif
