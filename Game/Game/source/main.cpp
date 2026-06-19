#include "../sqlite3/sqlite3.h"
#pragma comment(lib,"sqlite3/x64/sqlite3.lib")
#include <stdio.h>
#include <string>
#include "SqliteTableCreator.h"
#include "SqliteSeeder.h"

// sqlite3_exec()のコールバック関数
int select_response(
    void* get_prm,      // sqlite3_exec()の第4引数
    int col_cnt,        // 列数
    char** row_txt,     // 行内容
    char** col_name     // 列名
) 
{
    printf("%s : %s\n", row_txt[0], row_txt[1]);
    return 0;
}

int main() 
{
    //データベース接続先
    sqlite3* dbh = NULL;

    //データベース名
    const char* dbname = "newyosh.sqlite3";

    //実行結果
    int ret = -1;

    // エラー状態
    int err = 0;

    //データベースにアクセス（データベースが無いときは作成）
    ret = sqlite3_open_v2(
        dbname,
        &dbh,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        nullptr
    );
    if (ret == SQLITE_OK) 
    {
        printf("接続：成功\n");
    }
    else 
    {
        printf("接続：失敗(%d)\n", ret);
        err = 1;
    }

    // テーブルの生成
    if (err == 0)
    {
        if (CreateSqliteTables(dbh)) 
        {
            printf("テーブル生成：成功\n");
        }
        else 
        {
            printf("テーブル生成：失敗\n");
            //err = 1;
        }
    }

    // テーブルにデータを追加
    if (err == 0)
    {
        if (SeedSqliteData(dbh)) 
        {
            printf("データ追加：成功\n");
        }
        else 
        {
            printf("データ追加：失敗\n");
            err = 1;
        }
    }

    // テーブルのデータを取得する（callback）
    if (err == 0) 
    {
        char* errorMessage;
        ret = sqlite3_exec(dbh
            , "select * from book;"
            , select_response, NULL, &errorMessage);
        if (ret == SQLITE_OK) 
        {
            printf("データ取得：成功\n");
        }
        else 
        {
            printf("データ取得：失敗(%d)\n", ret);
            err = 1;
        }
    }

    //接続を切断
    ret = sqlite3_close(dbh);
    if (ret == SQLITE_OK) 
    {
        printf("切断：成功\n");
    }
    else 
    {
        printf("切断：失敗(%d)\n", ret);
        err = 1;
    }
    return 0;
}
 