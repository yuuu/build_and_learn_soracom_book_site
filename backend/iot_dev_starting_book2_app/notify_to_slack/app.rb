require 'json'
require 'slack-notifier'

def lambda_handler(event:, context:)
  webhookurl = ENV['IncomingWebhookUrl']
  return if webhookurl.nil?

  distance = event['distance']
  return if distance.nil?

  attachments = {
    attachments: [
      {
        color: '#999999',
        author_name: "TOFセンサー",
        title: "距離",
        text: "#{distance}[mm]",
      }
    ]
  }

  notifier = Slack::Notifier.new webhookurl
  notifier.ping attachments
end
