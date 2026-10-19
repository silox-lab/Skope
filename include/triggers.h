/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#ifndef TRIGGERS
#define TRIGGERS

#define DB_SCOPE_FILE_TRG1 \
"CREATE TRIGGER create_globalentity_sf " \
"AFTER INSERT ON ScopeFile " \
"FOR EACH ROW " \
"BEGIN " \
"INSERT INTO GlobalEntity (type, local_id, created_on) " \
"VALUES ('ScopeFile', NEW.id, unixepoch()); " \
"END;"

#define DB_SCOPE_FILE_TRG2 \
"CREATE TRIGGER delete_globalentity_sf " \
"AFTER DELETE ON ScopeFile " \
"FOR EACH ROW " \
"BEGIN " \
"DELETE FROM GlobalEntity WHERE local_id = OLD.id AND type = 'ScopeFile'; " \
"END;"

#define DB_SCOPE_TRG1 \
"CREATE TRIGGER create_globalentity_s " \
"AFTER INSERT ON Scope " \
"FOR EACH ROW " \
"BEGIN " \
"INSERT INTO GlobalEntity (type, local_id, created_on) " \
"VALUES ('Scope', NEW.id, unixepoch()); " \
"END;"

#define DB_SCOPE_TRG2 \
"CREATE TRIGGER delete_globalentity_s " \
"AFTER DELETE ON Scope " \
"FOR EACH ROW " \
"BEGIN " \
"DELETE FROM GlobalEntity WHERE local_id = OLD.id AND type = 'Scope'; " \
"END;"

#define DB_WIKI_TRG1 \
"CREATE TRIGGER create_globalentity_w " \
"AFTER INSERT ON Wiki " \
"FOR EACH ROW " \
"BEGIN " \
"INSERT INTO GlobalEntity (type, local_id, created_on) " \
"VALUES ('Wiki', NEW.id, unixepoch()); " \
"END;"

#define DB_WIKI_TRG2 \
"CREATE TRIGGER delete_globalentity_w " \
"AFTER DELETE ON Wiki " \
"FOR EACH ROW " \
"BEGIN " \
"DELETE FROM GlobalEntity WHERE local_id = OLD.id AND type = 'Wiki'; " \
"END;"

#endif