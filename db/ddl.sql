/*
  DDL for dynamic DB schema.
  DB contains data from sensors (detection reports)
*/

BEGIN

/* domains like those from static db (city_description) */
CREATE DOMAIN longitude AS numeric(12,9)
CHECK(
  VALUE between -180.0 and 180.0
);

CREATE DOMAIN latitude AS numeric(11,9)
CHECK(
  VALUE between -90.0 and 90.0
);

CREATE DOMAIN metersOverSea AS numeric(7,2)
CHECK(
  VALUE between -420.0 and 8850
);

COMMENT ON DOMAIN longitude IS 'As degrees (-180 -  180 with 9 positions after coma); +180 = 180E; -180 = 180W;';
COMMENT ON DOMAIN latitude IS 'As degrees (-90 - 90 with 9 positions after coma); +90 = 90N; -90 = 90S;';

CREATE TABLE DETECTION_REPORTS(
  SENSOR_ID integer not null,
  DR_ID SERIAL,
  LON longitude not null,
  LAT latitude not null,
  METERS_OVER_SEA metersOverSea not null,
  UPLOAD_TIME timestamp with time zone not null default current_timestamp,
  SENSOR_TIME timestamp with time zone not null,

  CONSTRAINT PK_DETECTION_REPORTS PRIMARY KEY(SENSOR_ID,DR_ID),
  CONSTRAINT FK_DETECTION_REPORTS_SENSOR FOREIGN KEY(SENSOR_ID) REFERENCES Sensors(sensorId)
);

COMMENT ON TABLE DETECTION_REPORTS IS 'Table contains detection reports provided by sensors (camers and others)';
COMMENT ON COLUMN DETECTION_REPORTS.LON IS 'Longitude (x) of where object was seen';
COMMENT ON COLUMN DETECTION_REPORTS.LAT IS 'Latitude (y) of where object was seen';
COMMENT ON COLUMN DETECTION_REPORTS.METERS_OVER_SEA IS 'Meters over sea (z) of where object was seen';
COMMENT ON COLUMN DETECTION_REPORTS.UPLOAD_TIME IS 'Time of data upload - when given DR appeared in system, its synchronized time for whole system';
COMMENT ON COLUMN DETECTION_REPORTS.SENSOR_TIME IS 'Time of data capture - when given DR was taken by sensor (its sensor time, NOT synchronized with whole system)';

CREATE TABLE FEATURE_VALUES(
  SENSOR_ID integer not null,
  DR_ID integer not null,
  FEATURE_DEFINITION_ID integer not null,
  VALUE varchar(255),

  CONSTRAINT PK_FEATURE_VALUES PRIMARY KEY(SENSOR_ID,DR_ID,FEATURE_DEFINITION_ID),
  CONSTRAINT FK_FEATURE_VALUES_DR FOREIGN KEY(SENSOR_ID,DR_ID) REFERENCES DETECTION_REPORTS(SENSOR_ID,DR_ID)  
);
CREATE INDEX FEATURE_DEFINITION_IDX ON FEATURE_VALUES(FEATURE_DEFINITION_ID);

COMMENT ON TABLE FEATURE_VALUES IS 'Table contains additional data obtained from sensors, like plates, colors etc.';
COMMENT ON COLUMN FEATURE_VALUES.VALUE IS 'Feature read serialized to string';

CREATE TABLE FEATURES_FOR_SENSORS(
  FEATURE_DEFINITION_ID integer not null,
  SENSOR_TYPE_ID integer not null,

  CONSTRAINT PK_FEATURES_FOR_SENSORS PRIMARY KEY(FEATURE_DEFINITION_ID,SENSOR_TYPE_ID),
  CONSTRAINT FK_FEATURES_FOR_SENSORS_DEF_ID FOREIGN KEY(FEATURE_DEFINITION_ID) REFERENCES FEATURE_DEFINITIONS(FEATURE_DEFINITION_ID),
  CONSTRAINT FK_FEATURES_FOR_SENSORS_ST_ID FOREIGN KEY(SENSOR_TYPE_ID) REFERENCES SensorTypes(typeId)
);
CREATE INDEX SENSOR_TYPES_IDX ON FEATURES_FOR_SENSORS(SENSOR_TYPE_ID);

COMMENT ON TABLE FEATURES_FOR_SENSORS IS 'M2M table which connects sensor types with feature definitions which they can provide';

CREATE TABLE FEATURE_DEFINITIONS(
  FEATURE_DEFINITION_ID SERIAL,
  NAME varchar(64) not null,
  TYPE varchar(32) not null,

  CONSTRAINT PK_FEATURE_DEFINITIONS PRIMARY KEY(FEATURE_DEFINITION_ID)
);

COMMENT ON TABLE FEATURE_DEFINITIONS IS 'Table contains types of features, which are available in system, e.g. plate, color, etc.';
COMMENT ON COLUMN FEATURE_DEFINITIONS.NAME IS 'Name of feature (like "color").';

CREATE TABLE TRACK_SNAPSHOTS(
  SNAPSHOT_ID integer not null,
  TRACK_ID char(36) not null, -- IDs from boost uuids
  UPLOAD_TIME timestamp with time zone not null default current_timestamp,
  LON longitude not null,
  LAT latitude not null,
  METERS_OVER_SEA metersOverSea not null,
  LON_VELOCITY numeric(13,9) not null default 0, -- according to current physics knowledge, there is nothing faster than light
  LAT_VELOCITY numeric(13,9) not null default 0,
  METERS_OVER_SEA_VELOCITY numeric(10,2) not null default 0,
  PREDICTED_LON longitude,
  PREDICTED_LAT latitude,
  PREDICTED_METERS_OVER_SEA metersOverSea,
  REFRESH_TIME timestamp with time zone not null,

  CONSTRAINT PK_TRACK_SNAPSHOTS PRIMARY KEY(SNAPSHOT_ID,TRACK_ID)
);

CREATE SEQUENCE TRACK_SNAPSHOT_SEQ;

-- Author: Andrzej 'fiedukow' Fiedukowicz
CREATE TABLE SensorTypes
(
  typeId SERIAL,
  typeName varchar(64) not null,

  CONSTRAINT pk_IdOfType PRIMARY KEY(typeId),
  CONSTRAINT uq_UniqueTypeName UNIQUE(typeName)
);

COMMENT ON TABLE SensorTypes IS 'Contains names of sensor types identifying parameters they are providing';
COMMENT ON COLUMN SensorTypes.typeId IS 'Unique ID of type of sensor';
COMMENT ON COLUMN SensorTypes.typeName IS 'Name of type of sensor - human readable';


END;
