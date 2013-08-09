class vertex
{
    public:

        vertex ()
        {
            x = 0;
            y = 0;
        }

        vertex (long double newX, long double newY)
        {
            x = newX;
            y = newY;
        }
        ~vertex (){}

        long double x;
        long double y;
};
