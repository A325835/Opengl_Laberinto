float skyboxVertices[] = 
{        
    -1.0f,  1.0f, -1.0f, 
    -1.0f, -1.0f, -1.0f, 
     1.0f, -1.0f, -1.0f, 
     1.0f,  1.0f, -1.0f, 
    -1.0f, -1.0f,  1.0f, 
    -1.0f,  1.0f,  1.0f, 
     1.0f, -1.0f,  1.0f, 
     1.0f,  1.0f,  1.0f,
};

unsigned int indicesSky[]{
    0, 1, 2, 2, 3, 0,
    4, 1, 0, 0, 5, 4, 
    2, 6, 7, 7, 3, 2,
    4, 5, 7, 7, 6, 4,
    0, 3, 7, 7, 5, 0,
    1, 4, 2, 2, 4, 6
};

// SKY
vector<string> faces
{
    /*
    right
    left
    top
    bottom
    front
    back

    o


    ft
    bk
    up
    dn
    rt
    lf

    */

    "Imagenes/Sky/elyvisions/dark_ft.png",
    "Imagenes/Sky/elyvisions/dark_bk.png",
    "Imagenes/Sky/elyvisions/dark_up.png",
    "Imagenes/Sky/elyvisions/dark_dn.png",
    "Imagenes/Sky/elyvisions/dark_rt.png",
    "Imagenes/Sky/elyvisions/dark_lf.png"
};
