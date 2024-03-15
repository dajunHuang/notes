SOURCE_DIR = $(PWD)
PANDOC = pandoc -s
STYLE = style.css
INSERT = insert_index.c
TIME = $(shell date)

SOURCES = $(wildcard $(SOURCE_DIR)/*.md)
OUTPUT = $(patsubst $(SOURCE_DIR)/%.md,$(SOURCE_DIR)/%.html,$(SOURCES))

FOLDERS = $(patsubst $(SOURCE_DIR)/%.md,$(SOURCE_DIR)/%,$(SOURCES))

.PHONY: all html index git clean

all: html index git 

html: $(OUTPUT) $(SOURCE_DIR)/$(STYLE)

$(OUTPUT): $(SOURCE_DIR)/%.html: $(SOURCE_DIR)/%.md
	$(PANDOC) $< -c $(STYLE) -o $@

index: $(SOURCE_DIR)/$(INSERT) $(SOURCE_DIR)/index_back.html $(SOURCES)
	gcc $(SOURCE_DIR)/$(INSERT) -o $(SOURCE_DIR)/$(patsubst %.c,%,$(INSERT)) -D__USE_XOPEN -D_GNU_SOURCE
	./$(patsubst %.c,%,$(INSERT)) $(SOURCE_DIR)/index_back.html $(patsubst $(SOURCE_DIR)/%.html,%.html,$(OUTPUT)) 

git:
	-git add .
	-git commit -m "$(TIME) Update site"
	git push -u -f origin master


clean:
	rm -f $(OUTPUT)
	rm -f $(SOURCE_DIR)/index.html
	rm -f $(SOURCE_DIR)/insert_index

test:
	echo $(FOLDERS)
	echo $(MESSAGE)
