#ifndef DEBUG_H
#define DEBUG_H

enum DebugM {
    ADELANTE,
    ATRAS,
    IZQUIERDA,
    DERECHA
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float gravity = -13.8f;


class Debug
{
public:
    vec3 Position;
    vec3 Front;
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;
    vec3 jumpVelocity = vec3(0.0f, 0.0f, 0.0f);

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    bool IsMouseLocked = true;
    bool isJumping = false;

    void LockMouse()
    {
        IsMouseLocked = true;
    }

    void UnlockMouse()
    {
        IsMouseLocked = false;
    }

    void ToggleMouseLock()
    {
        IsMouseLocked = !IsMouseLocked;
    }

    Debug(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
        IsMouseLocked = true;
    }

    Debug(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = vec3(posX, posY, posZ);
        WorldUp = vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
        IsMouseLocked = true;
    }

    mat4 GetViewMatrix()
    {
        return lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(DebugM direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == ADELANTE)
        {
            Position += Front * velocity;
        }
        if (direction == ATRAS)
        {
            Position -= Front * velocity;
        }
        if (direction == IZQUIERDA)
        {
            Position -= Right * velocity;
        }
        if (direction == DERECHA)
        {
            Position += Right * velocity;
        }
        if (!isJumping)
        {
            Position.y = 0.0;
        }
        Position.y = std::max(Position.y, 0.0f);
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        if (IsMouseLocked)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw += xoffset;
            Pitch += yoffset;

            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            updateCameraVectors();
        }

    }

    void ProcessMouseScroll(float yoffset)
    {
        if (IsMouseLocked)
        {
            if (Zoom >= 1.0f && Zoom <= 45.0f)
                Zoom -= yoffset;
            if (Zoom < 1.0f)
                Zoom = 1.0f;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }
    }

    void Jump(float jumpSpeed)
    {
        if (!isJumping) {
            isJumping = true;
            jumpVelocity.y = jumpSpeed;
        }
    }

    void Update(float deltaTime)
    {
        if (isJumping) {
            // Calcula la nueva posici�n vertical
            Position.y += jumpVelocity.y * deltaTime;
            jumpVelocity.y += gravity * deltaTime;

            // Verifica si el jugador ha llegado al suelo
            if (Position.y <= 0.0f) {
                Position.y = 0.0f;
                isJumping = false;
                jumpVelocity.y = 0.0f;
            }
        }
    }

private:
    void updateCameraVectors()
    {
        vec3 front;
        front.x = cos(radians(Yaw)) * cos(radians(Pitch));
        front.y = sin(radians(Pitch));
        front.z = sin(radians(Yaw)) * cos(radians(Pitch));
        Front = normalize(front);

        Right = normalize(cross(Front, WorldUp));
        Up = normalize(cross(Right, Front));
    }
};
#endif

