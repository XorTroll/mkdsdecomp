
NAME			:=	save-editor
OUTPUT_DIR		:=	../docs
OUTPUT			:=	$(NAME)
INDEX_HTML		:=	source/index.html
FAVICON			:= 	favicon.ico
INCLUDE			:=	-Iinclude
SOURCES			:=	$(shell find $(CURDIR)/source/ -type f -name "*.cpp")
ASSETS			:=	assets

IMGUI_DIR		:=	imgui

CXX				:=	emcc
CXX_FLAGS		:=	-std=c++20 -O1 -fexceptions

SOURCES			+=	$(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

LIBS			:=	-lGL
CXX_EMS_FLAGS	:=	-s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1 -s WASM=1 -s RETAIN_COMPILER_SETTINGS -s ASSERTIONS -s EXPORTED_FUNCTIONS="[_main, _malloc]" -s EXPORTED_RUNTIME_METHODS=[ccall]

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	mkdir -p $(OUTPUT_DIR)
	$(CXX) $(SOURCES) $(CXX_FLAGS) -o $(OUTPUT_DIR)/$(OUTPUT).js $(LIBS) $(CXX_EMS_FLAGS) --preload-file $(ASSETS) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends $(INCLUDE)
	cp $(INDEX_HTML) $(OUTPUT_DIR)/index.html
	cp $(FAVICON) $(OUTPUT_DIR)/favicon.ico

clean:
	rm -rf $(OUTPUT_DIR)
