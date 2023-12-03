#ifndef CAMERA_H
#define CAMERA_H

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    JUMP
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 3.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float JUMP_SPEED = 5.0f;


class Camera
{
public:
    vec3 Position;
    vec3 Front;
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float JumpStartY;
    const float GRAVITY = 9.81f;
    float JumpVelocity;
    bool IsMouseLocked = true;
    bool IsJumping = false;

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

    Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        JumpVelocity = 0.0f;
        IsJumping = false;
        updateCameraVectors();
        IsMouseLocked = true;
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
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

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
        {
            Position += Front * velocity;
        }
        if (direction == BACKWARD)
        {
            Position -= Front * velocity;
        }
        if (direction == LEFT)
        {
            Position -= Right * velocity;
        }
        if (direction == RIGHT)
        {
            Position += Right * velocity;
        }
        
    }

    void ProcessKeyboardFPS(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
        {
            vec3 newPosition = Position + Front * velocity;
            newPosition.y = Position.y;
            Position = newPosition;
        }
        if (direction == BACKWARD)
        {
            vec3 newPosition = Position - Front * velocity;
            newPosition.y = Position.y;
            Position = newPosition;
        }
        if (direction == LEFT)
        {
            vec3 newPosition = Position - Right * velocity;
            newPosition.y = Position.y;
            Position = newPosition;
        }
        if (direction == RIGHT)
        {
            vec3 newPosition = Position + Right * velocity;
            newPosition.y = Position.y;
            Position = newPosition;
        }
        if (direction == JUMP && !IsJumping)
        {
            IsJumping = true;
            JumpVelocity = JUMP_SPEED;
        }
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

   

    void UpdateJump(float deltaTime)
    {
        if (IsJumping)
        {
            Position.y += JumpVelocity * deltaTime;
            JumpVelocity -= GRAVITY * deltaTime;
            if (Position.y <= 0.0f)
            {
                Position.y = 0.0f;
                IsJumping = false;
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
