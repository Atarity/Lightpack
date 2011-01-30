

//


//

#ifndef DESKTOP_H
#define DESKTOP_H

class Desktop
{
public:
    Desktop();

public: // This variables set in Desktop constructor and using in all over classes
    // primary screen available geometry
    static int WidthAvailable;
    static int HeightAvailable;
    // full desktop available geometry
    static int WidthFull;
    static int HeightFull;
};

#endif // DESKTOP_H
