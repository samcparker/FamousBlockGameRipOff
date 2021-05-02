#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>

#include <noise/noise.h>
#include "noiseutils.h"
#include "camera.h"
#include "vec.h"



using namespace std;


class Face2D {
public:
    vector<vec2> points;
    vec3 normal;
    float distanceFromCamera;
    string direction;

    sf::Color colour = sf::Color::White;

    Face2D(float const&distanceFromCamera, vec3 normal) {
        this->distanceFromCamera = distanceFromCamera;
        this->normal = normal;
    };

    Face2D() = default;

    void add(vec2 point) {
        //cout << point << endl;
        points.push_back(point);
    }

    bool operator<(const Face2D& other) const {
        return (this->distanceFromCamera > other.distanceFromCamera);
    }
};


class Face {
public:
    vector<vec3> points;
    vec3 centroid;
    vec3 normal;
    string direction;
    float distanceToCamera;


    Face(vector<vec3> points, string direction) {
        this->points = points;

        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
        for (const vec3& p : points) {
            x += p.x;
            y += p.y;
            z += p.z;
        }
        x /= points.size();
        y /= points.size();
        z /= points.size();

        centroid = vec3(x, y, z);

        // Calculate normal
        vec3 a = points[2] - points[1];
        vec3 b = points[0] - points[1];
        this->normal = a.cross(b);

        this->direction = direction;
  

    };

    Face2D getPolygon(Camera camera, string direction) {

        Face2D face2d(centroid.distanceTo(camera.position), this->normal);



        for (vec3 a : points) {
            face2d.add(a.to2d(camera.position, camera.display_surface, camera.c_x, camera.c_y, camera.c_z, camera.s_x, camera.s_y, camera.s_z));
        }

        face2d.direction = direction;

        return face2d;
    }
};

class Cuboid {
public:
    vec3 position;
    vec3 size;
    vec3 centroid;

    vector<vec3> points;
    vector<Face> faces;

    Cuboid(vec3 position, vec3 size) {
        this->position = position;
        this->size = size;

        // Add all points of the cube

        this->update();

    };
    Cuboid() = default;

    void move(vec3 d) {
        this->position = position + d;
        //for (vec3 p : points) {
        //    p = p + d;
        //}
        //update();
        //cout << position << endl;
        update();
    }

    bool intersects(Cuboid b) {
        //return
        //    (abs(this->position.x - b.position.x) < this->size.x + b.size.x)
        //    &&
        //    (abs(this->position.y - b.position.y) < this->size.y + b.size.y)
        //    &&
        //    (abs(this->position.z - b.position.z) < this->size.z + b.size.z);

        return (this->position.x + this->size.x >= b.position.x && this->position.x <= b.position.x + b.size.x)
            && (this->position.y + this->size.y >= b.position.y && this->position.y <= b.position.y + b.size.y)
            && (this->position.z + this->size.z >= b.position.z && this->position.z <= b.position.z + b.size.z);

    }

    vector<Face2D> get2dPolygons(Camera camera) {
        vector<Face2D> faces2d;

        if (camera.position.distanceTo(this->centroid) > 40) {
            return faces2d;
        }

        // Sort faces by distance to centroid

        // Get distance to camera for each face

        for (int i = 0; i < faces.size(); i++) {
            faces[i].distanceToCamera = faces[i].centroid.distanceTo(camera.position);
        }

        sort(
            faces.begin(),
            faces.end(),
            [&](Face a, Face b) {return a.distanceToCamera < b.distanceToCamera; }
        );

        for (int i = 0; i < 3; i++) {
            // Check if depth > 1
            //float d = faces[i].centroid.distanceTo(camera.position).z;
            //blocked_directions

            // only calculate for non_blocked directions

            Face face = faces[i];

            /*  if (std::count(blocked_directions.begin(), blocked_directions.end(), face.direction)) {

              }*/

              // Get depth from camera
            vec3 a = face.centroid;
            float d = camera.getDepth(a);

            if (d > 1) {
                faces2d.push_back(face.getPolygon(camera, face.direction));
            }
        }

        return faces2d;
    };

private:
    void update() {
        updatePoints();
        updateFaces();
        updateCentroid();

    };
    void updatePoints() {
        this->points.clear();
        this->points.push_back(position + vec3(0, 0, 0)); // 0: 000
        this->points.push_back(position + vec3(0, 0, size.z)); // 1: 001
        this->points.push_back(position + vec3(0, size.y, 0)); // 2: 010
        this->points.push_back(position + vec3(0, size.y, size.z)); // 3: 011
        this->points.push_back(position + vec3(size.x, 0, 0)); // 4: 100
        this->points.push_back(position + vec3(size.x, 0, size.z)); // 5: 101
        this->points.push_back(position + vec3(size.x, size.y, 0)); // 6: 110
        this->points.push_back(position + vec3(size.x, size.y, size.z)); // 7: 111
    };
    void updateFaces() {
        // Make 6 faces of the cube, as if looking at the cube from the front

        // Front face
        this->faces.clear();

        this->faces.push_back(Face(vector<vec3>({
            this->points[0],
            this->points[2],
            this->points[6],
            this->points[4],
            }), "pz"));
        // Top face
        this->faces.push_back(Face(vector<vec3>({
            this->points[2],
            this->points[3],
            this->points[7],
            this->points[6],
            }), "py"));
        // Right face
        this->faces.push_back(Face(vector<vec3>({
            this->points[4],
            this->points[6],
            this->points[7],
            this->points[5],
            }), "px"));
        // Left face
        this->faces.push_back(Face(vector<vec3>({
            this->points[1],
            this->points[3],
            this->points[2],
            this->points[0],
            }), "nx"));
        // Bottom face
        this->faces.push_back(Face(vector<vec3>({
            this->points[1],
            this->points[5],
            this->points[4],
            this->points[0],
            }), "ny"));
        // Back face
        this->faces.push_back(Face(vector<vec3>({
            this->points[5],
            this->points[7],
            this->points[3],
            this->points[1],
            }), "nz"));
    };
    void updateCentroid() {
        vec3 centroid(0, 0, 0);
        for (const vec3& point : points) {
            centroid = centroid + point;
        }
        this->centroid = centroid / 8;
    };
};

class Cube : public Cuboid {
public:
    vec3 position;
    vector<vec3> points;
    vector<Face> faces;
    vec3 centroid;

    Cube(vec3 position) : Cuboid(position, vec3(1, 1, 1)) {};
    Cube() = default;

    //vector<vec2> get2dPoints(Camera camera) {
    //    vector<vec2> vecs;


    //    for (vec3 p : points) {
    //        vecs.push_back(p.to2d(camera.position, camera.display_surface, camera.c_x, camera.c_y, camera.c_z, camera.s_x, camera.s_y, camera.s_z));
    //    }

    //    return vecs;
    //    /* vecs;
    //    for (vec3 p : points) {
    //        vecs.push_back(p.to2d(camera));
    //    }
    //    return vecs;*/
    //}
    
private:
    typedef Cuboid super;
};

/*
Entity class.
Collides with walls.
Does not rotate so will use AABB collision detection.
*/
class Entity : public Cuboid {
public:
    float gravity = 9.81;
    float fall_time = 0;


    Entity(vec3 position, vec3 size, float gravity) : Cuboid(position, size) {
        this->gravity = gravity;
    }
    Entity() = default;

    vector<Face2D> getHitBoxFaces(Camera camera) {
        // Get HitBox faces

        // Take its position, and size and create Cube from that
        return get2dPolygons(camera);
    }

};

bool operator==(const vec3& lhs, const vec3& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<> struct std::hash<vec3> {
    std::size_t operator()(vec3 const& s) const noexcept {
        std::size_t h1 = std::hash<int>{}(s.x + s.z);
        std::size_t h2 = std::hash<int>{}(s.y + s.z);
        return h1 ^ (h2 << 1); // or use boost::hash_combine (see Discussion) https://en.cppreference.com/w/Talk:cpp/utility/hash
    }
};

// Move Entity in relation to the cube map by vec3 d
// Returns true if didn't have to prevent collision and false if it did
bool move(Entity &e, unordered_map<vec3, Cube> cube_map, vec3 d) {

    vec3 min(floor(e.position.x) - 1, floor(e.position.y) - 1, floor(e.position.z) - 1);
    vec3 max(ceil(e.position.x + e.size.x) + 1, ceil(e.position.y + e.size.y) + 1, ceil(e.position.z + e.size.z) + 1);

    vector<vec3> closeVec3s;
    // Choose all cubes within [min - 1, max + 1] and 

    for (int i = min.x; i <= max.x; i++) {
        for (int j = min.y; j <= max.y; j++) {
            for (int k = min.z; k <= max.z; k++) {
                closeVec3s.push_back(vec3(i, j, k));
            }
        }
    }

    vector<Cube> closeCubes;

    bool collided_y = false;

    for (const vec3& v3 : closeVec3s) {
        std::unordered_map<vec3, Cube>::const_iterator got = cube_map.find(v3);

        if (got != cube_map.end()) {
            closeCubes.push_back(got->second);
        }
    }

    e.move(vec3(d.x, 0, 0));
    for (Cube c : closeCubes) {
        if (e.intersects(c)) e.move(vec3(-d.x, 0, 0));
    }

    e.move(vec3(0, d.y, 0));
    for (Cube c : closeCubes) {
        //cout << "moving back" << endl;
        if (e.intersects(c)) {
            e.move(vec3(0, -d.y, 0));
            collided_y = true;
        }
    }

    e.move(vec3(0, 0, d.z));
    for (Cube c : closeCubes) {
        if (e.intersects(c)) e.move(vec3(0, 0, -d.z));
    }

    return collided_y;

}



int main()
{
    float speed = 4.317; // cubes/s

    // find speed in cubes/microseconds
    speed = speed / pow(10, 6);
    

    

    vec2 size(720, 720);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(size.x, size.y), "3D Thing", sf::Style::Default, settings);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    //Camera camera(vec3(0, 0, -10), vec3(0, 0, 0), vec3(0, 0, 3));
    //vec2 a();
    //vec3 a;
    Camera camera(vec3(0, 0, -10), vec3(0, 0, 0), vec3(0, 0, 1));   


    // Hide mouse
    window.setMouseCursorVisible(false);

    vector<Cube> cubes;
    unordered_map<vec3, Cube> cube_map;
   

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 1000; j++) {
            vec3 position = vec3(floor(i), floor(j), floor(j));
            Cube cube(position);
            pair<vec3, Cube> pair(position, cube);
            cube_map.insert(pair); 

        }
    }

    Entity e(vec3(0, -10, 10), vec3(0.5, 1.7, 0.5), 9.81);

    /*for (float i = 0.f; i < 30; i+=0.5f) {
        for (float j = 0.f; j < 15; j+=0.5f) {
        }
    }*/
    sf::Texture grassTop;
    grassTop.loadFromFile("SpriteSheet.png", sf::IntRect(0, 0, 16, 16));

    sf::Texture grassSide = grassTop;
    grassSide.loadFromFile("SpriteSheet.png", sf::IntRect(16, 0, 16, 16));

    sf::Texture grassBottom = grassTop;
    grassBottom.loadFromFile("SpriteSheet.png", sf::IntRect(32, 0, 16, 16));

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Time time1 = clock.getElapsedTime();
        //cout << time1.asMicroseconds() << endl;
        int dt = time1.asMicroseconds();

        float movement_speed = dt * speed;

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color(135, 206, 250));

        //set<Face2D> faces;
       

        camera.position = e.centroid + vec3(0, -1.5, 0);// + vec3(0, 1.5, 0);

        std::set<Face2D> faces;

        // Add entity to faces
        for (Face2D d : e.getHitBoxFaces(camera)) {
            //faces.insert(d);
        }

        for (auto& it : cube_map) {
            vec3 position = it.first;
            Cube cube = it.second;

            bool intersection = false;
            if (e.intersects(cube)) {
                intersection = true;
                //cout << "intersection" << endl;
            }

            vector<Face2D> fs = cube.get2dPolygons(camera);
            for (Face2D f : fs) {
                vec3 pos;
                if (f.direction == "px") {
                    pos = position + vec3(1, 0, 0);
                }
                else if (f.direction == "nx") {
                    pos = position + vec3(-1, 0, 0);
                }
                else if (f.direction == "py") {
                    pos = position + vec3(0, 1, 0);
                }
                else if (f.direction == "ny") {
                    pos = position + vec3(0, -1, 0);
                }
                else if (f.direction == "pz") {
                    pos = position + vec3(0, 0, -1);
                }
                else {// if (f.direction == "nz") {
                    pos = position + vec3(0, 0, 1);
                }

                std::unordered_map<vec3, Cube>::const_iterator got = cube_map.find(pos);
                if (intersection) f.colour = sf::Color::Red;

                if (got == cube_map.end()) {
                    faces.insert(f);
                }


            }
        }

        //for (Cube cube : cubes) {
        //    vector<Face2D> fs = cube.get2dPolygons(camera);
        //    for (Face2D f : fs) {
        //        faces.insert(f);
        //    }
        //}

        for (Face2D face : faces) {
            //sf::ConvexShape polygon(face.points.size());

            sf::VertexArray polygon(sf::Quads, 4);


            int i = 0;
            for (vec2 p : face.points) {
                polygon[i].position = sf::Vector2f(p.x * size.x * 0.5 + size.x * 0.5, p.y * size.y * 0.5 + size.y * 0.5);
                //polygon.setPoint(i, sf::Vector2f(p.x * size.x * 0.5 + size.x * 0.5, p.y * size.y * 0.5 + size.y * 0.5));
                i++;
            }
            polygon[0].texCoords = sf::Vector2f(15.f, 0.f);
            polygon[1].texCoords = sf::Vector2f(15.f, 15.f);
            polygon[2].texCoords = sf::Vector2f(0.f, 15.f);
            polygon[3].texCoords = sf::Vector2f(0.f, 0.f);


            polygon[0].color = face.colour;
            polygon[1].color = face.colour;
            polygon[2].color = face.colour;
            polygon[3].color = face.colour;

            if (face.direction == "ny") {
                window.draw(polygon, &grassTop);
                //polygon.setTexture(texture, true);
            }
            else if (face.direction == "py") {
                window.draw(polygon, &grassBottom);
            }
            else {
                
                window.draw(polygon, &grassSide);

            }

        }

        // Find change in mouse cursor from last frame to this one
        sf::Vector2i centre(window.getPosition().x + size.x / 2, window.getPosition().y + size.y / 2);
        sf::Vector2i pos = sf::Mouse::getPosition();

        int x_dif = pos.x - centre.x;
        int y_dif = pos.y - centre.y;

        camera.orientation = camera.orientation + vec3(-y_dif, x_dif, 0) * 0.005f;

        sf::Mouse::setPosition(centre);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) movement_speed *= 4;

        vec3 movement(0, 0, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement = movement + vec3(sin(camera.orientation.y) * 1, 0, cos(camera.orientation.y) * 1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement = movement + vec3(sin(camera.orientation.y) * -1, 0, cos(camera.orientation.y) * -1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement = movement + vec3(cos(camera.orientation.y) * -1, 0, sin(camera.orientation.y) * 1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement = movement + vec3(cos(camera.orientation.y) * 1, 0, sin(camera.orientation.y) * -1);
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) movement = movement + vec3(0, 1, 0);

        if (e.fall_time < 0.1) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) movement = movement + vec3(0, -1.5, 0);

        }

        
        
        movement = movement * movement_speed;

        e.fall_time += 0.01;


        move(e, cube_map, movement);

        bool collided_y = move(e, cube_map, vec3(0, 9.8 * e.fall_time * movement_speed, 0));
        if (collided_y) e.fall_time = 0;

        // gravity must also affect the entity
        

        window.display();
        camera.SetRotations();
    }

    return 0;
}