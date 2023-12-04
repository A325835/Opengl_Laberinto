#ifndef MY_COLLISION_CALLBACK_H
#define MY_COLLISION_CALLBACK_H

class MyCollisionCallback : public reactphysics3d::CollisionCallback {
public:
    bool areBodiesTouching = false; // Bandera para indicar si los cuerpos est�n toc�ndose

    void onContact(const CallbackData& callbackData) override {
        // Iterar a trav�s de los contactos
        for (uint32 i = 0; i < callbackData.getNbContactPairs(); ++i) {
            ContactPair contactPair = callbackData.getContactPair(i);

            if (contactPair.getNbContactPoints() > 0) {
                // Si hay al menos un punto de contacto, los cuerpos est�n toc�ndose
                areBodiesTouching = true;
                return; // No necesitamos seguir verificando m�s contactos
            }
        }
    }
};

#endif
