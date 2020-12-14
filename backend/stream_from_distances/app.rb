require 'json'
require 'aws-sdk'

def lambda_handler(event:, context:)
  records = event['Records']
  return if records.nil?

  records.each do |record|
    new_image, old_image = get_images(record)
    next if new_image.nil? || old_image.nil?

    imsi = new_image.dig('imsi', 'S')
    timestamp = new_image.dig('timestamp', 'N').to_i
    distance = new_image.dig('distance', 'N').to_i
    old_distance = old_image.dig('distance', 'N').to_i

    start(imsi, timestamp) if !attended?(old_distance) && attended?(distance)
    notify(imsi, timestamp) if attended?(old_distance) && attended?(distance)
    finish(imsi) if attended?(old_distance) && !attended?(distance)
  end
end

def get_images(record)
  new_image = record.dig('dynamodb', 'NewImage')
  old_image = record.dig('dynamodb', 'OldImage')
  [new_image, old_image]
end

def attended?(distance)
  distance < 1000
end

def start(imsi, timestamp)
  table_name = ENV['StartTimestampsTableName']
  return if table_name.nil?

  dynamodb = Aws::DynamoDB::Client.new(region: 'ap-northeast-1')
  dynamodb.put_item(
    table_name: table_name,
    item: { imsi: imsi, timestamp: timestamp }
  )

  iot_core_url = ENV['IotCoreUrl']
  return if iot_core_url.nil?

  iotcore = Aws::IoTDataPlane::Client.new(endpoint: iot_core_url)
  resp = iotcore.publish({
    topic: "messages/#{imsi}",
    qos: 0,
    payload: {message: 'Start'}.to_json,
  })
end

def notify(imsi, timestamp)
  table_name = ENV['StartTimestampsTableName']
  return if table_name.nil?

  dynamodb = Aws::DynamoDB::Client.new(region: 'ap-northeast-1')
  resp = dynamodb.get_item(
    table_name: table_name,
    key: { imsi: imsi }
  )

  iot_core_url = ENV['IotCoreUrl']
  return if iot_core_url.nil?

  iotcore = Aws::IoTDataPlane::Client.new(endpoint: iot_core_url)
  resp = iotcore.publish({
    topic: "messages/#{imsi}",
    qos: 0,
    payload: {message: "#{humanize(timestamp - resp.item['timestamp'])} passed"}.to_json,
  })
end

def finish(imsi)
  iot_core_url = ENV['IotCoreUrl']
  return if iot_core_url.nil?

  iotcore = Aws::IoTDataPlane::Client.new(endpoint: iot_core_url)
  resp = iotcore.publish({
    topic: "messages/#{imsi}",
    qos: 0,
    payload: {message: 'Finish'}.to_json,
  })
end

def humanize(msecs)
  values = msecs
  [[1000, :milliseconds], [60, :seconds], [60, :minutes], [24, :hours], [Float::INFINITY, :days]].map{ |count, name|
    next if values <= 0

    values, n = values.divmod(count)
    "#{n.to_i} #{n.to_i == 1 ? name.to_s.chop : name}" unless n.to_i == 0
  }.compact.last
end