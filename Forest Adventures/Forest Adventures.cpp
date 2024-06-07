# include <iostream>
# include <vector>
# include <ctime>
# include <fstream>
# include "SFML/Graphics.hpp"
# include "SFML\Audio.hpp"
# include "sfml\Window.hpp" 

using namespace std;
using namespace sf;

struct Bullet
{
    Sprite sprite;
    Vector2f velocity;
    Vector2f direction;
};

struct Enemy1
{
    Sprite sprite;
};
struct Enemy2
{
    Sprite sprite;
    int health;
};

//FUNCTION PROTOTYPE
void startSinglePlayerGame();
void startMultiplayerGame();
void showGuide(RenderWindow& gameWindow);
void showControls(RenderWindow& gameWindow, bool isMultiplayer);
Enemy1 spawnEnemy(const Texture& enemyTexture, const RenderWindow& gameWindow);
Enemy2 spawnEnemy2(const Texture& enemyTexture, const RenderWindow& gameWindow);

const float playerSpeed = 3.0f; // Fixed player movement speed
const float speed = 400.0f;
const float pi = 3.14f;
const float monsterSpeed = 15.0f;  // Speed of the monster
const float spiderSpeed = 20.0f;   // Speed of the spider

int main()
{
    RenderWindow window(VideoMode(900, 600), "Main Menu", Style::Default);
    window.setFramerateLimit(60);

    //Background Image
    Texture backgroundTexture;
    Sprite backgroundSprite;
    backgroundTexture.loadFromFile("Textures/background.png");
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setColor(Color(255, 255, 255, 255 * 0.5));      //Reduces opacity to 50%

    //Background Music
    Music bgMusic;
    bgMusic.openFromFile("Music/Main Menu.ogg");
    bgMusic.play();
    bgMusic.setVolume(50);

    //After Click Sound
    Music aclick;
    aclick.openFromFile("Sound effects/click.ogg");
    aclick.setVolume(100);

    //Font
    Font font;
    font.loadFromFile("Fonts/Coffee Spark.ttf");

    Text GameName;
    GameName.setFont(font);
    GameName.setCharacterSize(80.f);
    GameName.setPosition(120.f, 100.f);
    GameName.setFillColor(Color::White);
    GameName.setString("Forest Adventures");

    Text SubText;
    SubText.setFont(font);
    SubText.setCharacterSize(30.f);
    SubText.setPosition(350.f, 180.f);
    SubText.setFillColor(Color::White);
    SubText.setString(" Limited Edition");

    //SinglePlayer text
    Text player1Text;
    player1Text.setFont(font);
    player1Text.setCharacterSize(30.f);
    player1Text.setPosition(350.f, 300.f);
    player1Text.setString("SINGLEPLAYER");

    //Multiplayer text
    Text player2Text;
    player2Text.setFont(font);
    player2Text.setCharacterSize(30.f);
    player2Text.setPosition(350.f, 400.f);
    player2Text.setString("MULTIPLAYER");


    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))                      //ESCAPE or CROSS
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                window.close();
            }

            //Checks if Cursor is in Player1/Player2 (changes Color)
            Vector2i mousePos = Mouse::getPosition(window);
            if (player1Text.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
            {
                player1Text.setFillColor(Color::Red);
            }
            else
            {
                player1Text.setFillColor(Color::White);         // Set back to default color if the mouse is not over player1Text
            }
            if (player2Text.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
            {
                player2Text.setFillColor(Color::Red);
            }
            else
            {
                player2Text.setFillColor(Color::White);         // Set back to default color if the mouse is not over player1Text
            }

            //Openeing SinglePlayer/MultiPlayer Window
            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    if (player1Text.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        aclick.play();
                        bgMusic.stop();
                        window.close();
                        startSinglePlayerGame();
                    }
                    else if (player2Text.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        aclick.play();
                        bgMusic.stop();
                        window.close();
                        startMultiplayerGame();
                    }
                }
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(GameName);
        window.draw(SubText);
        window.draw(player1Text);
        window.draw(player2Text);
        window.display();
    }

    return 0;
}

/////////////////////////////////////////////////////////////
///                                                       ///
///                                                       ///
///                      SINGLEPLAYER                     ///
///                                                       ///
///                                                       ///
/////////////////////////////////////////////////////////////
void startSinglePlayerGame()
{
    const string highScoreFile = "singleplayer highscore.txt";
    int currentScore = 0;
    int highScore = 0;

    // Read the high score from the file
    ifstream readFile(highScoreFile);
    if (readFile.is_open())
    {
        readFile >> highScore;
        readFile.close();
    }

    RenderWindow gameWindow(VideoMode(900, 600), "Single Player Game");
    gameWindow.setFramerateLimit(60);
    srand(time(NULL));

    // Load player texture
    Texture playerTexture;
    playerTexture.loadFromFile("Textures/Player1.png");

    // Create player sprite
    Sprite player(playerTexture);
    player.setOrigin(playerTexture.getSize().x / 2, playerTexture.getSize().y / 2);
    player.setPosition(gameWindow.getSize().x / 2, gameWindow.getSize().y / 2);
    player.setScale(0.15f, 0.15f);
    Vector2f playercenter;

    // Load enemy texture
    Texture monsterTexture; //monster
    monsterTexture.loadFromFile("Textures/monster.png");
    Texture spiderTexture;  //spiders
    spiderTexture.loadFromFile("Textures/spider.png");

    // Vector to store monsters
    vector<Enemy1> monsters;

    // Vector to store spiders
    vector<Enemy2> spider;

    // Create some initial monsters
    for (int i = 0; i < 15; ++i)
    {
        monsters.push_back(spawnEnemy(monsterTexture, gameWindow));
    }

    // Create some initial spiders
    for (int i = 0; i < 7; i++)
    {
        spider.push_back(spawnEnemy2(spiderTexture, gameWindow));
    }

    // Load bullet texture
    Texture bulletTexture;
    bulletTexture.loadFromFile("Textures/bullet1.png");

    // Vector to store bullets
    vector<Bullet> bullets;
    int shootTimer = 0;

    //Clock to track time
    Clock clock;

    //Background
    Texture backgroundTexture;
    Sprite backgroundSprite;
    backgroundTexture.loadFromFile("Textures/Ingame_background.png");
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setColor(Color(255, 255, 255, 255 * 0.7));

    //Background Music
    Music bgMusic;
    bgMusic.openFromFile("Music/InGame.ogg");
    bgMusic.play();
    bgMusic.setVolume(50);

    //HighScore Music
    Music hsMusic;
    hsMusic.openFromFile("Sound effects/new highscore.ogg");
    hsMusic.setVolume(100);

    //Font
    Font font;
    font.loadFromFile("Fonts/Coffee Spark.ttf");

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);

    Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(24);
    highScoreText.setFillColor(Color::White);
    highScoreText.setPosition(0, 30);

    //Guide
    Texture questionMarkTexture;
    questionMarkTexture.loadFromFile("Textures/help.png");
    Sprite questionMarkSprite(questionMarkTexture);
    questionMarkSprite.setPosition(170, 10);
    questionMarkSprite.setScale(0.1f, 0.1f);

    // Controls Icon
    Texture controlsIconTexture;
    controlsIconTexture.loadFromFile("Textures/controls.png");
    Sprite controlsIconSprite(controlsIconTexture);
    controlsIconSprite.setScale(0.1f, 0.1f);  // Scale down the image to 10% of its original size
    controlsIconSprite.setPosition(220, 15);

    while (gameWindow.isOpen())
    {
        Event event;
        while (gameWindow.pollEvent(event))
        {
            if (event.type == Event::Closed)
                gameWindow.close();
        }

        float deltaTime = clock.restart().asSeconds();

        //Opening Guide
        Vector2i mousePos = Mouse::getPosition(gameWindow);
        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
        {
            if (questionMarkSprite.getGlobalBounds().contains(mousePos.x, mousePos.y))
            {
                showGuide(gameWindow);
            }
        }

        bool isMultiplayer = false;
        //Opening Controls
        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
        {

            if (controlsIconSprite.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
            {
                showControls(gameWindow, isMultiplayer);
            }
        }


        // Player movement
        Vector2f movement(0.0f, 0.0f);
        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            player.move(0, -playerSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            player.move(-playerSpeed, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            player.move(0, playerSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            player.move(playerSpeed, 0);
        }

        // Update player rotation
        float angle = 0;
        if (Keyboard::isKeyPressed(Keyboard::Right))
            player.rotate(angle + 5);
        if (Keyboard::isKeyPressed(Keyboard::Left))
            player.rotate(angle - 5);

        //Player Collision with Window
        if (player.getPosition().x <= 0)                       //LEFT
        {
            player.setPosition(0, player.getPosition().y);
        }
        if (player.getPosition().y <= 0)                       //UP
        {
            player.setPosition(player.getPosition().x, 0);
        }
        if (player.getPosition().x >= gameWindow.getSize().x)                         //RIGHT
        {
            player.setPosition(gameWindow.getSize().x, player.getPosition().y);
        }
        if (player.getPosition().y >= gameWindow.getSize().y)                        //BOTTOM
        {
            player.setPosition(player.getPosition().x, gameWindow.getSize().y);
        }

        // Shooting bullets
        Vector2f playercenter = player.getPosition();
        if (shootTimer < 20)
        {
            shootTimer++;
        }
        if (Keyboard::isKeyPressed(Keyboard::Space) && shootTimer >= 20)
        {
            Bullet newBullet;
            newBullet.sprite.setTexture(bulletTexture);
            newBullet.sprite.setOrigin(bulletTexture.getSize().x / 2, bulletTexture.getSize().y / 2);
            newBullet.sprite.setPosition(playercenter);

            // Bullet Rotation and Velocity
            float angle1 = (player.getRotation());
            newBullet.sprite.setRotation(angle1);

            // Convert angle to radians for velocity calculation
            float radianAngle = (angle1 - 90) * (3.142f / 180.f);
            newBullet.velocity.x = cos(radianAngle) * speed;
            newBullet.velocity.y = sin(radianAngle) * speed;

            bullets.push_back(newBullet);
            shootTimer = 0; // Reset shoot timer
        }

        //Bullet Move
        for (auto& bullet : bullets)
        {
            bullet.sprite.move(bullet.velocity * deltaTime);

        }

        // Update monsters
        for (auto& enemy1 : monsters)
        {
            Vector2f playerPosition = player.getPosition();
            Vector2f enemyPosition = enemy1.sprite.getPosition();
            Vector2f direction = playerPosition - enemyPosition;
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);
            direction /= length;
            enemy1.sprite.move(direction * monsterSpeed * deltaTime);

            // Set monsters rotation
            float enemyAngle = atan2(direction.y, direction.x) * 180 / pi;
            enemy1.sprite.setRotation(enemyAngle + 90); // +90 to face the correct direction
        }

        // Update spiders
        for (auto& enemy2 : spider)
        {
            Vector2f playerPosition = player.getPosition();
            Vector2f enemyPosition = enemy2.sprite.getPosition();
            Vector2f direction = playerPosition - enemyPosition;
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);
            direction /= length;
            enemy2.sprite.move(direction * spiderSpeed * deltaTime);

            // Set spiders rotation
            float enemyAngle = atan2(direction.y, direction.x) * 180 / pi;
            enemy2.sprite.setRotation(enemyAngle + 90); // +90 to face the correct direction
        }

        // Check for collisions between bullets and monsters
        for (size_t i = 0; i < bullets.size(); i++)
        {
            for (size_t k = 0; k < monsters.size(); k++)
            {
                if (bullets[i].sprite.getGlobalBounds().intersects(monsters[k].sprite.getGlobalBounds()))
                {
                    currentScore += 1;
                    monsters.erase(monsters.begin() + k);
                    bullets.erase(bullets.begin() + i);
                    monsters.push_back(spawnEnemy(monsterTexture, gameWindow)); // Spawn new enemy
                    break; // Exit inner loop
                }
            }
        }

        for (size_t i = 0; i < bullets.size(); i++)
        {
            bool bulletRemoved = false; // Track if bullet is removed
            for (size_t k = 0; k < spider.size() && !bulletRemoved; k++)
            {
                if (bullets[i].sprite.getGlobalBounds().intersects(spider[k].sprite.getGlobalBounds()))
                {
                    spider[k].health--;
                    bullets.erase(bullets.begin() + i);
                    bulletRemoved = true; // Bullet is removed
                    i--; // Adjust the bullet index after removal
                }

                // Check if the spider should be removed
                if (spider[k].health <= 0) {
                    currentScore += 1;
                    spider.erase(spider.begin() + k);
                    // Spawn new enemy
                    spider.push_back(spawnEnemy2(spiderTexture, gameWindow));
                    k--; // Adjust the spider index after removal
                }
            }
        }


        // Update text
        scoreText.setString("Score: " + to_string(currentScore));
        highScoreText.setString("High Score: " + to_string(highScore));

        gameWindow.clear();
        gameWindow.draw(backgroundSprite);
        gameWindow.draw(player);
        for (size_t i = 0; i < bullets.size(); i++)
        {
            gameWindow.draw(bullets[i].sprite);
        }
        for (size_t k = 0; k < monsters.size(); k++)
        {
            gameWindow.draw(monsters[k].sprite);
        }
        for (size_t j = 0; j < spider.size(); j++)
        {
            gameWindow.draw(spider[j].sprite);
        }
        gameWindow.draw(scoreText);
        gameWindow.draw(highScoreText);
        gameWindow.draw(questionMarkSprite);
        gameWindow.draw(controlsIconSprite);
        gameWindow.display();
    }

    // Check and update high score
    if (currentScore > highScore)
    {
        highScore = currentScore;
        hsMusic.play();

        // Write the new high score to the file
        ofstream writeFile(highScoreFile);
        if (writeFile.is_open())
        {
            writeFile << highScore;
            writeFile.close();
            cout << "New high score: " << highScore << endl;
        }
        else
        {
            cout << "Failed to write high score" << endl;
        }
    }
    else
    {
        cout << "High score remains: " << highScore << endl;
    }
}

/////////////////////////////////////////////////////////////
///                                                       ///
///                                                       ///
///                       MULTIPLAYER                     ///
///                                                       ///
///                                                       ///
/////////////////////////////////////////////////////////////

void startMultiplayerGame()
{
    const string highScoreFile = "multiplayer highscore.txt";
    int currentScore = 0;
    int highScore = 0;

    // Read the high score from the file
    ifstream readFile(highScoreFile);
    if (readFile.is_open())
    {
        readFile >> highScore;
        readFile.close();
    }

    RenderWindow gameWindow(VideoMode(900, 600), "Multiplayer Game");
    gameWindow.setFramerateLimit(60);

    //Background
    Texture backgroundTexture;
    Sprite backgroundSprite;
    backgroundTexture.loadFromFile("Textures/Ingame_background.png");
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setColor(Color(255, 255, 255, 255 * 0.7));

    //Background Music
    Music bgMusic;
    bgMusic.openFromFile("Music/InGame.ogg");
    bgMusic.play();
    bgMusic.setVolume(50);

    // Load bullet texture
    Texture bulletTexture;
    bulletTexture.loadFromFile("Textures/bullet1.png");

    // Vector to store bullets
    vector<Bullet> bullets;
    int shootTimer1 = 0;
    int shootTimer2 = 0;

    //Font
    Font font;
    font.loadFromFile("Fonts/Coffee Spark.ttf");

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);

    Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(24);
    highScoreText.setFillColor(Color::White);
    highScoreText.setPosition(0, 30);

    //Clock to track time
    Clock clock;

    // Load enemy texture
    Texture monsterTexture; //monster
    monsterTexture.loadFromFile("Textures/monster.png");
    Texture spiderTexture;  //spiders
    spiderTexture.loadFromFile("Textures/spider.png");

    // Vector to store monsters
    vector<Enemy1> monsters;

    // Vector to store spiders
    vector<Enemy2> spider;

    // Create some initial monsters
    for (int i = 0; i < 15; ++i)
    {
        monsters.push_back(spawnEnemy(monsterTexture, gameWindow));
    }

    // Create some initial spiders
    for (int i = 0; i < 10; i++)
    {
        spider.push_back(spawnEnemy2(spiderTexture, gameWindow));
    }

    //Guide
    Texture questionMarkTexture;
    questionMarkTexture.loadFromFile("Textures/help.png");
    Sprite questionMarkSprite(questionMarkTexture);
    questionMarkSprite.setPosition(170, 10);
    questionMarkSprite.setScale(0.1f, 0.1f);

    // Controls Icon
    Texture controlsIconTexture;
    controlsIconTexture.loadFromFile("Textures/controls.png");
    Sprite controlsIconSprite(controlsIconTexture);
    controlsIconSprite.setScale(0.1f, 0.1f);  // Scale down the image to 10% of its original size
    controlsIconSprite.setPosition(220, 15);

    /////////////////
    //   player1   //
    /////////////////

    // Load player1 texture
    Texture player1Texture;
    player1Texture.loadFromFile("Textures/Player1.png");

    // Create player1 sprite
    Sprite player1(player1Texture);
    player1.setOrigin(player1Texture.getSize().x / 2, player1Texture.getSize().y / 2);
    player1.setPosition(gameWindow.getSize().x / 2 - 100.f, gameWindow.getSize().y / 2);
    player1.setScale(0.15f, 0.15f);
    Vector2f player1center;

    /////////////////
    //   player2   //
    /////////////////

    // Load player2 texture
    Texture player2Texture;
    player2Texture.loadFromFile("Textures/Player2.png");

    // Create player2 sprite
    Sprite player2(player2Texture);
    player2.setOrigin(player2Texture.getSize().x / 2, player2Texture.getSize().y / 2);
    player2.setPosition(gameWindow.getSize().x / 2 + 100.f, gameWindow.getSize().y / 2);
    player2.setScale(0.21f, 0.21f);
    Vector2f player2center;

    while (gameWindow.isOpen())
    {
        Event event;
        while (gameWindow.pollEvent(event))
        {
            if (event.type == Event::Closed)
                gameWindow.close();
        }

        float deltaTime = clock.restart().asSeconds();

        Vector2i mousePos = Mouse::getPosition(gameWindow);
        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
        {
            if (questionMarkSprite.getGlobalBounds().contains(mousePos.x, mousePos.y))
            {
                showGuide(gameWindow);
            }
        }

        bool isMultiplayer = true;
        //Opening Controls
        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
        {

            if (controlsIconSprite.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
            {
                showControls(gameWindow, isMultiplayer);
            }
        }

        /////////////////
        //   player1   //
        /////////////////

        // Player movement
        Vector2f movement(0.0f, 0.0f);
        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            player1.move(0, -playerSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            player1.move(-playerSpeed, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            player1.move(0, playerSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            player1.move(playerSpeed, 0);
        }

        // Update player rotation
        float angle = 0;
        if (Keyboard::isKeyPressed(Keyboard::V))
            player1.rotate(angle + 5);
        if (Keyboard::isKeyPressed(Keyboard::X))
            player1.rotate(angle - 5);

        //Player Collision with Window
        if (player1.getPosition().x <= 0)                       //LEFT
        {
            player1.setPosition(0, player1.getPosition().y);
        }
        if (player1.getPosition().y <= 0)                       //UP
        {
            player1.setPosition(player1.getPosition().x, 0);
        }
        if (player1.getPosition().x >= gameWindow.getSize().x)                         //RIGHT
        {
            player1.setPosition(gameWindow.getSize().x, player1.getPosition().y);
        }
        if (player1.getPosition().y >= gameWindow.getSize().y)                        //BOTTOM
        {
            player1.setPosition(player1.getPosition().x, gameWindow.getSize().y);
        }

        // Shooting bullets
        Vector2f player1center = player1.getPosition();
        if (shootTimer1 < 20)
        {
            shootTimer1++;
        }
        if (Keyboard::isKeyPressed(Keyboard::C) && shootTimer1 >= 20)
        {
            Bullet newBullet;
            newBullet.sprite.setTexture(bulletTexture);
            newBullet.sprite.setOrigin(bulletTexture.getSize().x / 2, bulletTexture.getSize().y / 2);
            newBullet.sprite.setPosition(player1center);

            // Bullet Rotation and Velocity
            float angle1 = (player1.getRotation());
            newBullet.sprite.setRotation(angle1);

            // Convert angle to radians for velocity calculation
            float radianAngle = (angle1 - 90) * (3.142f / 180.f);
            newBullet.velocity.x = cos(radianAngle) * speed;
            newBullet.velocity.y = sin(radianAngle) * speed;

            bullets.push_back(newBullet);
            shootTimer1 = 0; // Reset shoot timer
        }

        //Bullet Move
        for (auto& bullet : bullets)
        {
            bullet.sprite.move(bullet.velocity * deltaTime);

        }

        /////////////////
        //   player2   //
        /////////////////

        // Player movement
        if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            player2.move(0, -playerSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            player2.move(-playerSpeed, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            player2.move(0, playerSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            player2.move(playerSpeed, 0);
        }

        // Update player rotation
        if (Keyboard::isKeyPressed(Keyboard::P))
            player2.rotate(angle + 5);
        if (Keyboard::isKeyPressed(Keyboard::I))
            player2.rotate(angle - 5);

        //Player Collision with Window
        if (player2.getPosition().x <= 0)                       //LEFT
        {
            player2.setPosition(0, player2.getPosition().y);
        }
        if (player2.getPosition().y <= 0)                       //UP
        {
            player2.setPosition(player2.getPosition().x, 0);
        }
        if (player2.getPosition().x >= gameWindow.getSize().x)                         //RIGHT
        {
            player2.setPosition(gameWindow.getSize().x, player2.getPosition().y);
        }
        if (player2.getPosition().y >= gameWindow.getSize().y)                        //BOTTOM
        {
            player2.setPosition(player2.getPosition().x, gameWindow.getSize().y);
        }

        // Shooting bullets
        Vector2f player2center = player2.getPosition();
        if (shootTimer2 < 20)
        {
            shootTimer2++;
        }
        if (Keyboard::isKeyPressed(Keyboard::O) && shootTimer2 >= 20)
        {
            Bullet newBullet;
            newBullet.sprite.setTexture(bulletTexture);
            newBullet.sprite.setOrigin(bulletTexture.getSize().x / 2, bulletTexture.getSize().y / 2);
            newBullet.sprite.setPosition(player2center);

            // Bullet Rotation and Velocity
            float angle1 = (player2.getRotation());
            newBullet.sprite.setRotation(angle1);

            // Convert angle to radians for velocity calculation
            float radianAngle = (angle1 - 90) * (3.142f / 180.f);
            newBullet.velocity.x = cos(radianAngle) * speed;
            newBullet.velocity.y = sin(radianAngle) * speed;

            bullets.push_back(newBullet);
            shootTimer2 = 0; // Reset shoot timer
        }

        //Bullet Move
        for (auto& bullet : bullets)
        {
            bullet.sprite.move(bullet.velocity * deltaTime);

        }

        // Update monsters 50/50 player 1 and player 2
        for (auto& enemy1 : monsters)
        {
            size_t halfSize = monsters.size() / 2;
            Vector2f player1Position = player1.getPosition();
            Vector2f player2Position = player2.getPosition();

            for (size_t i = 0; i < monsters.size(); ++i)
            {
                Vector2f enemyPosition = monsters[i].sprite.getPosition();
                Vector2f direction;
                if (i < halfSize)
                {
                    // First half follows player1
                    direction = player1Position - enemyPosition;
                }
                else
                {
                    // Second half follows player2
                    direction = player2Position - enemyPosition;
                }

                float length = sqrt(direction.x * direction.x + direction.y * direction.y);
                direction /= length;
                monsters[i].sprite.move(direction * (monsterSpeed - 14.f) * deltaTime);

                // Set monster rotation
                float enemyAngle = atan2(direction.y, direction.x) * 180 / pi;
                monsters[i].sprite.setRotation(enemyAngle + 90); // +90 to face the correct direction
            }

        }

        // Update spiders 50/50 player 1 and player 2
        for (auto& enemy2 : spider)
        {
            size_t halfSize = spider.size() / 2;
            Vector2f player1Position = player1.getPosition();
            Vector2f player2Position = player2.getPosition();

            for (size_t i = 0; i < spider.size(); ++i)
            {
                Vector2f enemyPosition = spider[i].sprite.getPosition();
                Vector2f direction;
                if (i < halfSize)
                {
                    // First half follows player1
                    direction = player1Position - enemyPosition;
                }
                else
                {
                    // Second half follows player2
                    direction = player2Position - enemyPosition;
                }

                float length = sqrt(direction.x * direction.x + direction.y * direction.y);
                direction /= length;
                spider[i].sprite.move(direction * (spiderSpeed - 19.f) * deltaTime);

                // Set spider rotation
                float enemyAngle = atan2(direction.y, direction.x) * 180 / pi;
                spider[i].sprite.setRotation(enemyAngle + 90); // +90 to face the correct direction
            }


        }

        // Check for collisions between bullets and monsters
        for (size_t i = 0; i < bullets.size(); i++)
        {
            for (size_t k = 0; k < monsters.size(); k++)
            {
                if (bullets[i].sprite.getGlobalBounds().intersects(monsters[k].sprite.getGlobalBounds()))
                {
                    currentScore += 1;
                    monsters.erase(monsters.begin() + k);
                    bullets.erase(bullets.begin() + i);
                    monsters.push_back(spawnEnemy(monsterTexture, gameWindow)); // Spawn new enemy
                    break; // Exit inner loop
                }
            }
        }

        for (size_t i = 0; i < bullets.size(); i++)
        {
            bool bulletRemoved = false; // Track if bullet is removed
            for (size_t k = 0; k < spider.size() && !bulletRemoved; k++)
            {
                if (bullets[i].sprite.getGlobalBounds().intersects(spider[k].sprite.getGlobalBounds()))
                {
                    spider[k].health--;
                    bullets.erase(bullets.begin() + i);
                    bulletRemoved = true; // Bullet is removed
                    i--; // Adjust the bullet index after removal
                }

                // Check if the spider should be removed
                if (spider[k].health <= 0) {
                    currentScore += 1;
                    spider.erase(spider.begin() + k);
                    // Spawn new enemy
                    spider.push_back(spawnEnemy2(spiderTexture, gameWindow));
                    k--; // Adjust the spider index after removal
                }
            }
        }

        // Update text
        scoreText.setString("Score: " + to_string(currentScore));
        highScoreText.setString("High Score: " + to_string(highScore));

        gameWindow.clear();
        gameWindow.draw(backgroundSprite);
        gameWindow.draw(player1);
        gameWindow.draw(player2);
        for (size_t i = 0; i < bullets.size(); i++)
        {
            gameWindow.draw(bullets[i].sprite);
        }
        for (size_t k = 0; k < monsters.size(); k++)
        {
            gameWindow.draw(monsters[k].sprite);
        }
        for (size_t j = 0; j < spider.size(); j++)
        {
            gameWindow.draw(spider[j].sprite);
        }
        gameWindow.draw(scoreText);
        gameWindow.draw(highScoreText);
        gameWindow.draw(questionMarkSprite);
        gameWindow.draw(controlsIconSprite);
        gameWindow.display();
    }

    // Check and update high score
    if (currentScore > highScore)
    {
        highScore = currentScore;

        // Write the new high score to the file
        ofstream writeFile(highScoreFile);
        if (writeFile.is_open())
        {
            writeFile << highScore;
            writeFile.close();
            cout << "New high score: " << highScore << endl;
        }
        else
        {
            cout << "Failed to write high score" << endl;
        }
    }
    else
    {
        cout << "High score remains: " << highScore << endl;
    }
}

Enemy1 spawnEnemy(const Texture& enemyTexture, const RenderWindow& gameWindow)
{
    Enemy1 monster;
    monster.sprite.setTexture(enemyTexture);
    monster.sprite.setOrigin(enemyTexture.getSize().x / 2, enemyTexture.getSize().y / 2);

    // Spawn at random position on the edge of the screen(monsters)
    int edge = rand() % 4;
    if (edge == 0)
    {
        monster.sprite.setPosition(rand() % gameWindow.getSize().x, 0); // Left edge
    }
    else if (edge == 1)
    {
        monster.sprite.setPosition(rand() % gameWindow.getSize().x, gameWindow.getSize().y); // Bottom edge
    }
    else if (edge == 2)
    {
        monster.sprite.setPosition(0, rand() % gameWindow.getSize().y); // Top edge
    }
    else
    {
        monster.sprite.setPosition(gameWindow.getSize().x, rand() % gameWindow.getSize().y); // Right edge
    }

    return monster;
}

Enemy2 spawnEnemy2(const Texture& enemyTexture, const RenderWindow& gameWindow)
{
    Enemy2 spiders;
    spiders.sprite.setTexture(enemyTexture);
    spiders.sprite.setOrigin(enemyTexture.getSize().x / 2, enemyTexture.getSize().y / 2);
    spiders.sprite.setScale(0.15f, 0.15f);
    spiders.health = 3;
    // Spawn at random position on the edge of the screen(spiders)
    int edge = rand() % 4;
    if (edge == 0)
    {
        spiders.sprite.setPosition(rand() % gameWindow.getSize().x, 0); // Left edge
    }
    else if (edge == 1)
    {
        spiders.sprite.setPosition(rand() % gameWindow.getSize().x, gameWindow.getSize().y); // Bottom edge
    }
    else if (edge == 2)
    {
        spiders.sprite.setPosition(0, rand() % gameWindow.getSize().y); // Top edge
    }
    else
    {
        spiders.sprite.setPosition(gameWindow.getSize().x, rand() % gameWindow.getSize().x); // Right edge
    }

    return spiders;
}

void showGuide(RenderWindow& gameWindow)
{
    RenderWindow guideWindow(VideoMode(500, 300), "Guide");

    //Background Image
    Texture backgroundTexture;
    Sprite backgroundSprite;
    backgroundTexture.loadFromFile("Textures/fairy forest.jpg");
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setColor(Color(255, 255, 255, 255 * 0.5));      //Reduces opacity to 50%

    Font font;
    font.loadFromFile("Fonts/Coffee Spark.ttf");

    Text guideText;
    guideText.setFont(font);
    guideText.setCharacterSize(20);
    guideText.setFillColor(Color::White);
    guideText.setString("Guide:\nKill as many monsters and spiders as you can.\n"
        "Beat the high score without dying.");
    guideText.setPosition(10, 10);

    Text backButton;
    backButton.setFont(font);
    backButton.setCharacterSize(20);
    backButton.setFillColor(Color::White);
    backButton.setString("Back");
    backButton.setPosition(10, guideWindow.getSize().y - 30);

    while (guideWindow.isOpen()) {
        Event event;
        while (guideWindow.pollEvent(event))
        {
            if (event.type == Event::Closed)
                guideWindow.close();

            Vector2i mousePos = Mouse::getPosition(guideWindow);
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    guideWindow.close();
                }
            }
        }

        guideWindow.clear();
        guideWindow.draw(backgroundSprite);
        guideWindow.draw(guideText);
        guideWindow.draw(backButton);
        guideWindow.display();
    }
}

void showControls(RenderWindow& gameWindow, bool isMultiplayer)
{
    RenderWindow controlsWindow(VideoMode(500, 500), "Controls");

    // Background Image
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("Textures/fairy forest.jpg");
    Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setColor(Color(255, 255, 255, 128)); // Reduces opacity to 50%
    backgroundSprite.setScale(
        float(controlsWindow.getSize().x) / backgroundTexture.getSize().x,
        float(controlsWindow.getSize().y) / backgroundTexture.getSize().y
    );

    Font font;
    font.loadFromFile("Fonts/Coffee Spark.ttf");

    Text controlsText;
    controlsText.setFont(font);
    controlsText.setCharacterSize(20);
    controlsText.setFillColor(Color::White);

    if (isMultiplayer)
    {
        controlsText.setString("Multiplayer Controls: \n\n"
            "Player 1:\n\t WASD to move\n \tX to rotate left\n \tV to rotate Right\n \tC to Shoot.\n\n"
            "Player 2:\n \tArrow keys to move\n \tI to rotate Left\n \tP to rotate Right\n \tO to Shoot.");
    }
    else
    {
        controlsText.setString("Singleplayer Controls:\n\n"
            "\tWASD keys to move\n \tLeft Arrow to rotate left\n \tRight Arrow to rotate Right\n \tSpace to shoot.");
    }
    controlsText.setPosition(10, 10);

    Text backButton;
    backButton.setFont(font);
    backButton.setCharacterSize(20);
    backButton.setFillColor(Color::White);
    backButton.setString("Back");
    backButton.setPosition(10, controlsWindow.getSize().y - 30);

    while (controlsWindow.isOpen())
    {
        Event event;
        while (controlsWindow.pollEvent(event))
        {
            if (event.type == Event::Closed)
                controlsWindow.close();

            Vector2i mousePos = Mouse::getPosition(controlsWindow);
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    controlsWindow.close();
                    showControls(gameWindow, isMultiplayer);
                }
            }
        }

        controlsWindow.clear();
        controlsWindow.draw(backgroundSprite);
        controlsWindow.draw(controlsText);
        controlsWindow.draw(backButton);
        controlsWindow.display();
    }
}