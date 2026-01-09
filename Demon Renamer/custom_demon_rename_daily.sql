CREATE TABLE IF NOT EXISTS custom_demon_rename_daily (
  owner INT UNSIGNED NOT NULL,
  last_rename_date DATE NOT NULL,
  PRIMARY KEY (owner)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
