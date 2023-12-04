#ifndef MY_COLLISION_CALLBACK_H
#define MY_COLLISION_CALLBACK_H

class MyCollisionCallback : public reactphysics3d::CollisionCallback {
public:
    bool areBodiesTouching = false; // Bandera para indicar si los cuerpos están tocándose

    void onContact(const CallbackData& callbackData) override {
        // Iterar a través de los contactos
        for (uint32 i = 0; i < callbackData.getNbContactPairs(); ++i) {
            ContactPair contactPair = callbackData.getContactPair(i);

            if (contactPair.getNbContactPoints() > 0) {
                // Si hay al menos un punto de contacto, los cuerpos están tocándose
                areBodiesTouching = true;
                return; // No necesitamos seguir verificando más contactos
            }
        }
    }
};

#endif
