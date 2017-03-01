########################################################################
# =  単純展開変数   宣言したタイミングで値が代入される
# := 再帰属展開変数 実行時に値が代入される
# += 値をアペンドする。 
#    単純にアペンドするのではなく、先頭に空欄(半角スペース)をつけてから値をアペンドする。
# 例)
# HOGE=$(FUGA) ←実行時にFUGAが代入される
# FUGA=aa
# FUGA=bb
# all:
# 	@echo $(HOGE) ← aa bbが表示される(aaとbbの間に空欄が付加されている)
#
#------------------------------------------------------------------------
# フォルダ構成例)
# MemSample(ワークスペース配下)
#    |_ memmain.c
#    |_ samplefunc.c
#
# * : マッチしたものがスペース区切りの文字列として渡される
#  取得値 = memmain.c samplefunc.c
#
# % : マッチしたものが1つずつループで渡されるイメージ
#  取得値 = 1回目=memmain.c 2回目=samplefunc.c
#------------------------------------------------------------------------
#
# $@ : ターゲット名
# $^ : ターゲットの依存する全てのファイルを取得
# $< : ターゲットの依存するファイルの最初のものだけを取得する
#     例) $^: memmain.c samplefunc.c   =>   $<: memmain.c
#
# $% : ターゲットがアーカイブメンバだったときのターゲットメンバ名
# $? : ターゲットより新しいすべての依存するファイル名(要は依存ファイルの中で更新されたものだけを取得する)
# $+ : Makefileと同じ順番の依存するファイルの名前
# $* : ターゲット名からサフィックスを除いた名前
# 
# \ 処理が複数行に続く場合、末尾に\を付加する
# 
# - コマンドの先頭につけるとコマンド実行時、エラーが発生した場合でも正常終了したと見なす。
#   それにより、処理が中断されず、以降のコマンドが実行される。
# @ コマンドの先頭につけると実行コマンドを標準出力へ表示されなくなる。
# 
# <参考>
# https://www.ecoop.net/coop/translated/GNUMake3.77/make_toc.jp.html
# https://www.ecoop.net/coop/translated/GNUMake3.77/make_10.jp.html
# http://d.hatena.ne.jp/goth_wrist_cut/20080317/1205769293
# http://blog.wagavulin.jp/entry/20120405/1333629926
#
# コンパイル情報
# [全体的なオプション]
# -c    : コンパイルのみ行う(オブジェクトファイルを生成する)
# -o    : コンパイルした際に生成されるオブジェクトファイルの出力先を指定する。
#         (指定しない場合、カレントディレクトリへa.outという名前でファイル出力する)
#
# [警告オプション]
# -Wall : 警告オプションをすべて有効にする(http://at-aka.blogspot.jp/2006/11/gcc-wall-w.html)
# -W    : 警告メッセージをより詳しく表示する
#         時折チェックが望まれる項目は警告を表示させる
# -fmessage-length=n : メッセージの長さを指定(nを超えた場合、折り返す)
#
# [デバッグオプション]
# -g    : 実行ファイルにデバッグ情報を埋め込む(gdb等で使用する)
#
# [最適化オプション]
# -O0   : コンパイル時、コードの最適化を行わない
#          => 開発時はこのオプションを設定する
#             (最適化をするとコードの入れ替えや削除が行われ、gdbでのデバッグが難しくなる為。)
# -O2   : コンパイル時、コードの最適化を行う
#          => 本番環境適用時はこのオプションを設定する
#             (最適化を行うことで処理速度向上させるため)
#
# [プリプロセッサオプション]
# -M    : オブジェクト間の依存関係をする。
# -MM   : システムヘッダファイルは除外して表示する。 
# -MD, MMD  : 上記に加え、コンパイルも行う。(ただ、-cも設定しないとだめみたい)
# -MF   : 依存関係を標準出力でなく、指定したファイルへ出力する
# -MP   : 依存するヘッダファイルを偽のターゲットとして追加する。
########################################################################

# ターゲット名
TARGET=	SocketChatClientGUI

# デバッグモード(定義をコメントアウトをすると無効化)
DEBUG=

# コンパイラ
COMPILER=	gcc

# コンパイルオプション
#  <デバッグモード有効時>
#    ・コードの最適化無効
#    ・デバッグ情報の埋込
ifdef DEBUG
COMPILE_OPT_LINK=	-O2 -W -Wall -o
COMPILE_OPT=	-O2 -W -Wall -c
else
COMPILE_OPT_LINK=	-O0 -W -Wall -o
COMPILE_OPT=	-O0 -g3 -W -Wall -c
endif


# ソースファイル
SRC_DIR= ./
SRCS=	$(wildcard $(SRC_DIR)*.c)

# オブジェクトファイル
OBJ_DIR=	./Debug/
OBJS=	$(addprefix $(OBJ_DIR),$(notdir $(SRCS:.c=.o)))

# 依存関係ファイル
DEPS_DIR=	./Debug/depents/
DEPS=	$(addprefix $(DEPS_DIR),$(notdir $(SRCS:.c=.d)))

# 実行ファイル
EXE=	$(OBJ_DIR)$(TARGET).exe

LIBS=	-l"comctl32" -l"user32" -l"gdi32" -l"WinMM" -l"ws2_32"

##############################################################################
# リソースファイル
RESS_DIR=	./resource/
RESS=	$(wildcard $(RESS_DIR)*.rc)

# オブジェクトファイル
RESS_OBJS=	$(addprefix $(OBJ_DIR),$(notdir $(RESS:.rc=.rco)))

# 依存関係ファイル
RESS_DEPS=	$(addprefix $(DEPS_DIR),$(notdir $(RESS:.rc=.rcd)))
##############################################################################

all:	$(TARGET)

# オブジェクトファイルをリンカし、実行ファイルを生成する
$(TARGET): $(OBJS) $(RESS_OBJS)
	$(COMPILER) $(COMPILE_OPT_LINK) $(EXE) $^ $(LIBS)


# 各cファイルをコンパイルし、オブジェクトファイルを生成する
# また各オブジェクトファイルの依存関係をファイルへ出力する
# -D"UNICODE=3" -D"_UNICODE=3"
# $(OBJS): $(SRCS)              =>  毎回 main.c memcfuncs2.c memfuncs.cが取得される ←　だからこれだとだめ
# $(OBJ_DIR)%.o: $(SRC_DIR)%.c  =>  1回目：main.c 2回目：memcfuncs2.c 3回目：memfuncs.c
#-D"UNICODE" -D"_UNICODE" 
$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@[ -d $(OBJ_DIR) ] || mkdir $(OBJ_DIR)
	@[ -d $(DEPS_DIR) ] || mkdir $(DEPS_DIR)
	
	$(COMPILER) $(COMPILE_OPT) -MMD -MP -MF"$(DEPS_DIR)$(<F:.c=.d)" -MT"$(@)" -o $@ $<


$(OBJ_DIR)%.rco: $(RESS_DIR)%.rc
	@[ -d $(OBJ_DIR) ] || mkdir $(OBJ_DIR)
	@[ -d $(DEPS_DIR) ] || mkdir $(DEPS_DIR)
	
	windres $< $@
	
AAAAAAA:
	$(COMPILER) $(SRC_DIR)socketClientGui.c


clean:
	rm -f $(EXE) $(OBJS) $(DEPS) $(RESS_OBJS) $(RESS_DEPS)


# 依存関係の追加
#  => これによりヘッダーファイルのみ更新した場合でも、
#     依存するファイルがリコンパイル・リンカされる
-include $(DEPS) $(RESS_DEPS)


.PHONY: all clean $(TARGET)

