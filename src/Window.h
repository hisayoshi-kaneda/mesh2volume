#ifdef _MSC_VER
#pragma once
#endif

#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"

#ifdef _WIN32
#define WINDOWPOS 100 // For Windows
#else
#define WINDOWPOS 100 // For MacOS or Linux
#endif

enum ArcballMode {
	ARCBALL_MODE_NONE = 0x00,
	ARCBALL_MODE_TRANSLATE = 0x01,
	ARCBALL_MODE_ROTATE = 0x02,
	ARCBALL_MODE_SCALE = 0x04
};

class Window {
public:
	GLFWwindow* window = nullptr;
	int width;
	int height;
	const string win_name;

	// Arcballコントロールのための変数
	bool isDragging = false;

	int arcballMode = ARCBALL_MODE_NONE;
	glm::mat4 modelMat, viewMat, projMat;
	glm::mat4 acRotMat, acTransMat, acScaleMat;
	glm::vec3 gravity;
	glm::vec3 rotCenter;

	float acScale = 1.0f;
	glm::ivec2 oldPos;
	glm::ivec2 newPos;

	Window(const int width, const int height, const char* window_name)
		: width(width), height(height), win_name(window_name) {
		initialize();
	}

	virtual ~Window() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	explicit operator bool() {
		// ウィンドウを閉じる必要がなければ true を返す
		return !glfwWindowShouldClose(window);
	}

	void initialize() {
		// GLFW を初期化する
		if (glfwInit() == GL_FALSE) {
			std::cerr << "Can't initialize GLFW" << std::endl;
			exit(1);
		}

#ifdef _WIN32
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#elif __APPLE__
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, win_name.c_str(), nullptr, nullptr);
		if (!window) {
			std::cerr << "Can't create GLFW window." << std::endl;
			abort();
		}

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);
		glfwSetWindowPos(window, WINDOWPOS, 100);

		// OpenGL 3.x/4.xの関数をロードする (glfwMakeContextCurrentの後でないといけない)
		const int version = gladLoadGL(glfwGetProcAddress);
		if (version == 0) {
			fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
			exit(1);
		}

		// バージョンを出力する
		printf("Vendor: %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("OpenGL: %s\n", glGetString(GL_VERSION));
		printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		glfwSetWindowUserPointer(window, this);

		//resizeの設定
		auto resize_callback = [](GLFWwindow* w, int width, int height) {
			reinterpret_cast<Window*>(glfwGetWindowUserPointer(w))->resize(width, height);
		};
		glfwSetWindowSizeCallback(window, resize_callback);

		// マウスのイベントを処理する関数を登録
		auto mouseEvent_callback = [](GLFWwindow* w, int button, int action, int moods) {
			reinterpret_cast<Window*>(glfwGetWindowUserPointer(w))->mouseEvent(button, action, moods);
		};
		glfwSetMouseButtonCallback(window, mouseEvent_callback);
		auto mouseMoveEvent_callback = [](GLFWwindow* w, double xpos, double ypos) {
			reinterpret_cast<Window*>(glfwGetWindowUserPointer(w))->mouseMoveEvent(xpos, ypos);
		};
		glfwSetCursorPosCallback(window, mouseMoveEvent_callback);
		auto wheelEvent_callback = [](GLFWwindow* w, double xpos, double ypos) {
			reinterpret_cast<Window*>(glfwGetWindowUserPointer(w))->wheelEvent(xpos, ypos);
		};
		glfwSetScrollCallback(window, wheelEvent_callback);

		// OpenGLの初期設定
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// カメラの初期化
		projMat = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		viewMat = glm::lookAt(glm::vec3(3.0f, 4.0f, 5.0f),  // 視点の位置
			glm::vec3(0.0f, 0.0f, 0.0f),  // 見ている先
			glm::vec3(0.0f, 1.0f, 0.0f)); // 視界の上方向


		//その他の行列の初期化
		modelMat = glm::mat4(1.0);
		acRotMat = glm::mat4(1.0);
		acTransMat = glm::mat4(1.0);
		acScaleMat = glm::mat4(1.0);
		gravity = glm::vec3(0, 0, 0);
		rotCenter = glm::vec3(0, 0, 0);
	}


	void flush() {
		glfwSwapBuffers(window);
		glfwPollEvents();
		//glfwWaitEvents();
	}

	virtual void draw() {
	}

	virtual void main_loop() {
		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			draw();
			flush();
		}
	}

	glm::mat4 mvMat() {
		return viewMat * modelMat * acTransMat  * acScaleMat * acRotMat;
	}

	glm::mat4 mvpMat() {
		return  projMat * mvMat();
	}

private:
	virtual void resize(int _width, int _height) {
		width = _width;
		height = _height;
		glfwSetWindowSize(window, _width, _height);
		int renderBufferWidth, renderBufferHeight;
		glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);
		glViewport(0, 0, renderBufferWidth, renderBufferHeight);
		// 投影変換行列の初期化
		projMat = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////ARCBALL_CONTROL///////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void mouseEvent(int button, int action, int mods) {
		// クリックしたボタンで処理を切り替える
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			arcballMode = ARCBALL_MODE_ROTATE;
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			arcballMode = ARCBALL_MODE_SCALE;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			arcballMode = ARCBALL_MODE_TRANSLATE;
		}

		// クリックされた位置を取得
		double px, py;
		glfwGetCursorPos(window, &px, &py);

		if (action == GLFW_PRESS) {
			if (!isDragging) {
				isDragging = true;
				oldPos = glm::ivec2(px, py);
				newPos = glm::ivec2(px, py);
			}
		}
		else {
			isDragging = false;
			oldPos = glm::ivec2(0, 0);
			newPos = glm::ivec2(0, 0);
			arcballMode = ARCBALL_MODE_NONE;
		}
	}

	// スクリーン上の位置をアークボール球上の位置に変換する関数
	glm::vec3 getVector(double x, double y) {
		glm::vec3 pt(2.0 * x / width - 1.0,
			-2.0 * y / height + 1.0, 0.0);
		const double xySquared = pt.x * pt.x + pt.y * pt.y;
		if (xySquared <= 1.0) {
			// 単位円の内側ならz座標を計算
			pt.z = std::sqrt(1.0 - xySquared);
		}
		else {
			// 外側なら球の外枠上にあると考える
			pt = glm::normalize(pt);
		}
		return pt;
	}

	void updateRotate() {
		// スクリーン座標をアークボール球上の座標に変換
		const glm::vec3 u = glm::normalize(getVector(newPos.x, newPos.y));
		const glm::vec3 v = glm::normalize(getVector(oldPos.x, oldPos.y));

		// カメラ座標における回転量 (=オブジェクト座標における回転量)
		const double angle = std::acos(std::max(-1.0f, std::min(glm::dot(u, v), 1.0f)));

		// カメラ空間における回転軸
		const glm::vec3 rotAxis = glm::cross(v, u);

		// カメラ座標の情報をワールド座標に変換する行列
		const glm::mat4 c2oMat = glm::inverse(viewMat * modelMat);

		// オブジェクト座標における回転軸
		const glm::vec3 rotAxisObjSpace = glm::vec3(c2oMat * glm::vec4(rotAxis, 0.0f));

		// 回転行列の更新
		acRotMat = glm::translate(rotCenter)*glm::rotate((float)(4.0 * angle), rotAxisObjSpace)*glm::translate(-rotCenter) * acRotMat;
	}

	void updateTranslate() {
		// オブジェクト重心のスクリーン座標を求める
		glm::vec4 gravityScreenSpace = (projMat * viewMat * modelMat) * glm::vec4(gravity.x, gravity.y, gravity.z, 1.0f);
		gravityScreenSpace /= gravityScreenSpace.w;

		// スクリーン座標系における移動量
		glm::vec4 newPosScreenSpace(2.0 * newPos.x / width, -2.0 * newPos.y / height, gravityScreenSpace.z, 1.0f);
		glm::vec4 oldPosScreenSpace(2.0 * oldPos.x / width, -2.0 * oldPos.y / height, gravityScreenSpace.z, 1.0f);

		// スクリーン座標の情報をオブジェクト座標に変換する行列
		const glm::mat4 s2oMat = glm::inverse(projMat * viewMat * modelMat);

		// スクリーン空間の座標をオブジェクト空間に変換
		glm::vec4 newPosObjSpace = s2oMat * newPosScreenSpace;
		glm::vec4 oldPosObjSpace = s2oMat * oldPosScreenSpace;
		newPosObjSpace /= newPosObjSpace.w;
		oldPosObjSpace /= oldPosObjSpace.w;

		// オブジェクト座標系での移動量
		const glm::vec3 transObjSpace = glm::vec3(newPosObjSpace - oldPosObjSpace);

		// オブジェクト空間での平行移動
		acTransMat = glm::translate(acTransMat, transObjSpace);
	}

	void updateScale() {
		acScaleMat = glm::scale(glm::vec3(acScale, acScale, acScale));
	}

	void updateMouse() {
		switch (arcballMode) {
		case ARCBALL_MODE_ROTATE:
			updateRotate();
			break;

		case ARCBALL_MODE_TRANSLATE:
			updateTranslate();
			break;

		case ARCBALL_MODE_SCALE:
			acScale += (float)(oldPos.y - newPos.y) / height;
			updateScale();
			break;
		}
	}

	void mouseMoveEvent(double xpos, double ypos) {
		if (isDragging) {
			// マウスの現在位置を更新
			newPos = glm::ivec2(xpos, ypos);

			// マウスがあまり動いていない時は処理をしない
			const double dx = newPos.x - oldPos.x;
			const double dy = newPos.y - oldPos.y;
			const double length = dx * dx + dy * dy;
			if (length < 2.0f * 2.0f) {
				return;
			}
			else {
				updateMouse();
				oldPos = glm::ivec2(xpos, ypos);
			}
		}
	}

	void wheelEvent(double xpos, double ypos) {
		acScale += ypos / 10.0;
		updateScale();
	}
};

#endif //WINDOW_H
